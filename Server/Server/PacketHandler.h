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
				//const uint16 PacketSize{ sizeof(T) };
				//if (data.size() < sizeof(T))
				//{
				//	return;
				//}

				T Pkt{ Serializer::Deserialize<T>(data) };
				logic_func(session, Pkt);
			}
		);
	}

	template <typename T>
	static void RegisterDynamicHandler(
		Common::PacketType type,
		std::function<void(const T&, const uint8*, const uint16)> logic_func)
	{
		_handlerMap.emplace(
			type,
			[logic_func](const std::vector<uint8>& Data)
			{
				// 방어: 버퍼 크기가 최소 헤더 크기도 안 되면 무시
				const uint16 PacketSize{ sizeof(T) };
				if (Data.size() < sizeof(T))
				{
					return;
				}

				// 고정 데이터 파싱
				T Packet{ Serializer::Deserialize<T>(Data) };
				const uint16 TotalSize{ Packet.size };

				// 가변 데이터 파싱
				const uint8* PayloadPtr{ Data.data() + PacketSize };
				const uint16 PayloadSize{ static_cast<uint16>(TotalSize - PacketSize) };

				logic_func(Packet, PayloadPtr, PayloadSize);
			}
		);
	}

	// todo: thread unsafe한 것들 정상화 시키기
	// --
	// static handler
	// --

	static void HandleMovePlayer(SessionPtr session, const Common::CSMovePlayer& packet);
	static void HandleLogin(SessionPtr session, const Common::CSLogin& packet);
	static void HandleGetRoomList(SessionPtr session, const Common::CSGetRoomList& packet);
	static void HandleCreateRoom(SessionPtr session, const Common::CSCreateRoom& packet);
	static void HandleJoinRoom(SessionPtr session, const Common::CSJoinRoom& packet);
	static void HandleReady(SessionPtr session, const Common::CSReady& packet);
	static void HandleStartGame(SessionPtr session, const Common::CSStartGame& packet);

	// --
	// dynamic handler
	// --
	


	using HandlerFunc = std::function<void(SessionPtr, const RecvBuffer&)>;
	static std::unordered_map<Common::PacketType, HandlerFunc> _handlerMap;

};