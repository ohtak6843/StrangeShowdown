#pragma once
#include "Session.h"
#include "Serializer.h"

class PacketHandler
{
public:
	static void Init();

	static void HandlePacket(SessionPtr session, const RecvBuffer& data);

private:
    static void HandleMovePlayer(SessionPtr session, const packet::CSMovePlayer& packet);
	static void HandleLogin(SessionPtr session, const packet::CSLogin& packet);

	template <typename T>
	static void RegisterHandler(packet::Type type, std::function<void(SessionPtr, const T&)> logic_func)
	{
		_handlerMap.emplace(type, [logic_func](SessionPtr session, const RecvBuffer& data) {
			T Pkt{ Serializer::Deserialize<T>(data) };
			logic_func(session, Pkt);
			});
	}


	using HandlerFunc = std::function<void(SessionPtr, const RecvBuffer&)>;
	static std::unordered_map<packet::Type, HandlerFunc> _handlerMap;

};