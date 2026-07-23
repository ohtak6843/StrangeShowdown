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
	static void RegisterHandler(
		Common::PacketType type,
		std::function<void(SessionPtr, const T&)> logic_func)
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

	template <typename T>
	static void RegisterDynamicHandler(
		Common::PacketType type,
		std::function<void(SessionPtr, const T&, const uint8*, const uint16)> logic_func)
	{
		_handlerMap.emplace(
			type,
			[logic_func](SessionPtr session, const RecvBuffer& Data)
			{
				// 고정 데이터 파싱
				T Packet{ Serializer::Deserialize<T>(Data) };
				const uint16 TotalSize{ Packet.size };

				// 가변 데이터 파싱
				const uint16 PacketSize{ sizeof(T) };
				const uint8* PayloadPtr{ Data.data() + PacketSize };
				const uint16 PayloadSize{ static_cast<uint16>(TotalSize - PacketSize) };

				logic_func(session, Packet, PayloadPtr, PayloadSize);
			}
		);
	}


	
	// --
	// static handler
	// --
	// todo: thread unsafe한 것들 정상화 시키기
	static void HandleMovePlayer(SessionPtr session, const Common::CSMovePlayer& packet);
	static void HandleLogin(SessionPtr session, const Common::CSLogin& packet);
	static void HandleGetRoomList(SessionPtr session, const Common::CSGetRoomList& packet);
	static void HandleCreateRoom(SessionPtr session, const Common::CSCreateRoom& packet);
	static void HandleJoinRoom(SessionPtr session, const Common::CSJoinRoom& packet);
	static void HandleReady(SessionPtr session, const Common::CSReady& packet);
	static void HandleStartGame(SessionPtr session, const Common::CSStartGame& packet);
	static void HandleUseItem(SessionPtr session, const Common::CSUseItem& packet);
	static void HandlePickItem(SessionPtr session, const Common::CSPickItem& packet);

	// --
	// dynamic handler
	// --
	static void HandleChat(SessionPtr session, const Common::CSChat& packet,
		const uint8* payload, const uint16 payload_size);


	using HandlerFunc = std::function<void(SessionPtr, const RecvBuffer&)>;
	static std::unordered_map<Common::PacketType, HandlerFunc> _handlerMap;

};