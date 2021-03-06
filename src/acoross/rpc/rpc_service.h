#ifndef ACOROSS_RPC_SERVICE_H_
#define ACOROSS_RPC_SERVICE_H_

#include <SDKDDKVer.h>
#include <boost/asio.hpp>
#include <memory>
#include <functional>
#include <unordered_map>

#include <acoross/rpc/rpc_packet.h>
#include <acoross/rpc/rpc_socket.h>

namespace acoross {
namespace rpc {

using boost::asio::ip::tcp;

//---------------------------------------------//

class ProcedureCallerBase
{
public:
	virtual bool operator()(RpcPacket& request_packet, std::shared_ptr<RpcPacket> reply_packet) = 0;
};

//---------------------------------------------//

template <typename RqT, typename RpT>
class ProcedureCaller : public ProcedureCallerBase
{
public:
	using ProcedureF = std::function<rpc::ErrCode(RqT&, RpT*)>;

	explicit ProcedureCaller(ProcedureF func)
		: ProcedureCallerBase()
		, func_(func)
	{}

	virtual bool operator()(RpcPacket& request_packet, std::shared_ptr<RpcPacket> reply_packet)
	{
		RqT request_msg;
		if (request_msg.ParseFromArray(request_packet.body(), request_packet.body_length()) == false)
		{
			return false;
		}

		RpT reply_msg;
		auto err = func_(request_msg, &reply_msg);

		reply_packet->encode_header(request_packet.message_type(), reply_msg.ByteSize(), request_packet.get_header().rpc_msg_uid_);
		return reply_msg.SerializeToArray(reply_packet->body(), reply_packet->max_body_length);
	}

private:
	ProcedureF func_;
};

//---------------------------------------------//

using ProcedureTable = std::unordered_map<unsigned short, std::shared_ptr<ProcedureCallerBase>>;

class RpcService
	: public RpcSocket
{
public:
	RpcService(::boost::asio::io_service& io_service, tcp::socket&& socket)
		: RpcSocket(io_service, std::move(socket))
	{	
	}
	virtual ~RpcService() 
	{}

protected:
	// <message type, message handler>
	ProcedureTable procedures_;

private:
	virtual bool process_msg(RpcPacket& request_packet) override
	{
		auto rpc_msg_uid = request_packet.get_header().rpc_msg_uid_;
		auto err_code = request_packet.get_header().error_code_;

		auto it = procedures_.find(request_packet.message_type());
		if (it == procedures_.end())
		{
			return false;
		}

		auto procedure = it->second;
		auto reply_packet = std::make_shared<RpcPacket>();
		bool ret = (*procedure)(request_packet, reply_packet);
		if (ret == false)
		{
			return false;
		}

		send(reply_packet);
		return true;
	}
};

}
}
#endif //ACOROSS_RPC_SERVICE_H_