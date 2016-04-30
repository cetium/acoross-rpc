#ifndef ACOROSS_RPC_SOCKET_H_
#define ACOROSS_RPC_SOCKET_H_

#include <SDKDDKVer.h>
#include <boost/asio.hpp>
#include <memory>
#include <deque>
#include <atomic>

#include "rpc_packet.h"

namespace acoross {
namespace rpc {

using boost::asio::ip::tcp;

class RpcSocket
	: public std::enable_shared_from_this<RpcSocket>
{
public:
	RpcSocket(::boost::asio::io_service& io_service, tcp::socket&& socket,
		std::function<void(boost::system::error_code ec)> end_handler = std::function<void(boost::system::error_code ec)>())
		: io_service_(io_service)
		, socket_(std::move(socket))
		, write_strand_(io_service)
		, end_handler_(end_handler)
	{
	}

	virtual ~RpcSocket()
	{
		size_t cnt = 0;
		cnt += write_msgs_.size();
		RpcSocket::pending_write().fetch_sub(my_pending_write_.load());
		auto cnt2 = RpcSocket::pending_write_cnt().load();
		cnt2++;
	}

	void start()
	{
		do_read_header();
		started_.store(true);
	}

	void end(boost::system::error_code ec)
	{
		//_ASSERT(0);
		if (end_handler_)
		{
			end_handler_(ec);
		}
	}

	void send(std::shared_ptr<RpcPacket> new_msg);

	static std::atomic<size_t>& pending_write()
	{
		static std::atomic<size_t> pending_write_ = 0;
		return pending_write_;
	}
	static std::atomic<size_t>& pending_write_cnt()
	{
		static std::atomic<size_t> pending_write_cnt = 0;
		return pending_write_cnt;
	}

protected:
	virtual bool process_msg(RpcPacket& msg) = 0;

private:
	void do_write();
	void do_read_header();
	void do_read_body();

private:
	::boost::asio::io_service& io_service_;
	tcp::socket socket_;	//rpc 전용 소켓
	
	RpcPacket read_msg_;

	std::function<void(boost::system::error_code ec)> end_handler_;

	//
	::boost::asio::strand write_strand_;
	std::deque<std::shared_ptr<RpcPacket>> write_msgs_;
	std::atomic<size_t> my_pending_write_{ 0 };
	//

	std::atomic<bool> started_{ false };
};

}
}

#include "rpc_socket.ipp"

#endif