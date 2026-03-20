#pragma once
#include "Session.h"
#include "Serializer.h"
#include "Protocol.h"

class PacketHandler
{
public:
	static void Init();

	static void HandlePacket(SessionPtr session, const RecvBuffer& data);

private:
	template <typename T>
	static void RegisterHandler(Common::PacketType type, std::function<void(SessionPtr, const T&)> logic_func)
	{
		_handlerMap.emplace(
			type,
			[logic_func](SessionPtr session, const RecvBuffer& data)
			{
				T Pkt{ Serializer::Deserialize<T>(data) };
				logic_func(session, Pkt);
			}
		);
	}

	// todo: thread unsafe한 것들 정상화 시키기
	static void HandleMovePlayer(SessionPtr session, const Common::CSMovePlayer& packet);
	static void HandleLogin(SessionPtr session, const Common::CSLogin& packet);
	static void HandleGetRoomList(SessionPtr session, const Common::CSGetRoomList& packet);
	static void HandleCreateRoom(SessionPtr session, const Common::CSCreateRoom& packet);
	static void HandleJoinRoom(SessionPtr session, const Common::CSJoinRoom& packet);




	using HandlerFunc = std::function<void(SessionPtr, const RecvBuffer&)>;
	static std::unordered_map<Common::PacketType, HandlerFunc> _handlerMap;

};