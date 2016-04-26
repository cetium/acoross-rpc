#ifndef ACOROSS_RPC_SOCKET_IPP_
#define ACOROSS_RPC_SOCKET_IPP_
//#include "rpc_socket.h"

namespace acoross {
namespace rpc {

inline void RpcSocket::send(std::shared_ptr<RpcPacket> new_msg)
{
	if (started_.load() == false)
	{
		_ASSERT(0);
		return abort();
	}

	RpcSocket::pending_write().fetch_add(new_msg->length());
	my_pending_write_.fetch_add(new_msg->length());
	RpcSocket::pending_write_cnt().fetch_add(1);

	write_strand_.post(
		[this, new_msg]()
	{
		bool write_in_progress = !write_msgs_.empty();
		write_msgs_.push_back(new_msg);
		if (!write_in_progress)
		{
			do_write();
		}
	});
}

inline void RpcSocket::do_write()
{
	auto self(shared_from_this());
	boost::asio::async_write(socket_,
		boost::asio::buffer(write_msgs_.front()->data(),
			write_msgs_.front()->length()),
		[this, self](boost::system::error_code ec, std::size_t length)
	{
		if (!ec)
		{
			write_strand_.post(
				[this]()
			{
				RpcSocket::pending_write().fetch_sub(write_msgs_.front()->length());
				my_pending_write_.fetch_sub(write_msgs_.front()->length());
				RpcSocket::pending_write_cnt().fetch_sub(1);

				write_msgs_.pop_front();

				if (!write_msgs_.empty())
				{
					do_write();
				}

			});
		}
		else
		{
			end(ec);
		}
	});
}

inline void RpcSocket::do_read_header()
{
	auto self(shared_from_this());
	boost::asio::async_read(socket_,
		boost::asio::buffer(read_msg_.data(), RpcPacket::header_length),
		[this, self](boost::system::error_code ec, std::size_t /*length*/)
	{
		if (!ec && read_msg_.decode_header())
		{
			do_read_body();
		}
		else
		{
			end(ec);
		}
	});
}

inline void RpcSocket::do_read_body()
{
	auto self(shared_from_this());
	boost::asio::async_read(socket_,
		boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
		[this, self](boost::system::error_code ec, std::size_t /*length*/)
	{
		if (!ec && process_msg(read_msg_))
		{
			do_read_header();
		}
		else
		{
			end(ec);
		}
	});
}

}
}
#endif //ACOROSS_RPC_SOCKET_IPP_