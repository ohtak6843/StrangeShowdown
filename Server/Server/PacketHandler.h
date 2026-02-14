#pragma once
#include "Session.h"
#include "Serializer.h"
#include "protocol.h"

class PacketHandler
{
public:
	static void Init();

	static void HandlePacket(SessionPtr session, const RecvBuffer& data);

private:
	template <typename T>
	static void RegisterHandler(Common::PacketType type, std::function<void(SessionPtr, const T&)> logic_func)
	{
		_handlerMap.emplace(type, [logic_func](SessionPtr session, const RecvBuffer& data) {
			T Pkt{ Serializer::Deserialize<T>(data) };
			logic_func(session, Pkt);
			});
	}

	static void HandleMovePlayer(SessionPtr session, const Common::CSMovePlayer& packet);
	static void HandleLogin(SessionPtr session, const Common::CSLogin& packet);
	static void HandleGetRoomList(SessionPtr session, const Common::CSGetRoomList& packet);


	using HandlerFunc = std::function<void(SessionPtr, const RecvBuffer&)>;
	static std::unordered_map<Common::PacketType, HandlerFunc> _handlerMap;

};