#include "pch.h"
#include "PacketHandler.h"
#include "RoomManager.h"
#include "IOCP.h"
#include "Serializer.h"

std::unordered_map<Common::PacketType, PacketHandler::HandlerFunc> PacketHandler::_handlerMap;

void PacketHandler::Init()
{
	// 핸들러 등록
	RegisterHandler<Common::CSLogin>(
		Common::PacketType::CS_LOGIN,
		[](SessionPtr session, const auto& packet) {
			HandleLogin(session, packet);
		}
	);

	RegisterHandler<Common::CSMovePlayer>(
		Common::PacketType::CS_MOVE_PLAYER,
		[](SessionPtr session, const auto& packet) {
			HandleMovePlayer(session, packet);
		}
	);

	RegisterHandler<Common::CSGetRoomList>(
		Common::PacketType::CS_GET_ROOM_LIST,
		[](SessionPtr session, const auto& packet) {
			HandleGetRoomList(session, packet);
		}
	);
}

void PacketHandler::HandlePacket(SessionPtr session, const RecvBuffer& buffer)
{
	if (nullptr == session)
	{
		return;
	}

	Common::Header& header{ *reinterpret_cast<Common::Header*>(const_cast<int8*>(buffer.data())) };
	auto type{ header.type };
	auto iter{ _handlerMap.find(type) };
	if (_handlerMap.end() != iter)
	{
		auto& handler_func{ iter->second };
		handler_func(session, buffer);
	}
}

void PacketHandler::HandleMovePlayer(SessionPtr session, const Common::CSMovePlayer& packet)
{
	auto id{ session->GetSessionID() };
	auto player{ GET_SINGLE(RoomManager)->GetPlayer(id) };

	player->HandleMove(packet);

	// 다른 플레이어에게도 전파
	auto player_map{ GET_SINGLE(RoomManager)->GetRoom(id)->GetPlayers() };

	for (const auto& [other_id, other_player] : player_map)
	{
		if (other_id == id)
		{
			continue;
		}

		// 다른 플레이어에게 내 위치 전달
		Common::SCMovePlayer move_object_packet{
			id,
			player->GetPosition(),
			player->GetDirection(),
			player->GetState()
		};
		other_player->GetOwnerSession()->DoSend(move_object_packet);
	}
}

void PacketHandler::HandleLogin(SessionPtr session, const Common::CSLogin& packet)
{
	auto id{ session->GetSessionID() };
	auto player{ GET_SINGLE(RoomManager)->GetPlayer(id) };
	auto player_map{ GET_SINGLE(RoomManager)->GetRoom(id)->GetPlayers() };

	for (const auto& [other_id, other_player] : player_map)
	{
		if (other_id == id)
		{
			continue;
		}

		// 다른 플레이어에게 내 위치 전달
		Common::SCSpawnObject move_object_packet{
			id,
			player->GetPosition(),
			player->GetDirection(),
		};
		other_player->GetOwnerSession()->DoSend(move_object_packet);

		// 현재 클라이언트에 기존 플레이어 정보 전달
		Common::SCSpawnObject other_spawn_packet{
			other_id,
			other_player->GetPosition(),
			other_player->GetDirection()
		};
		session->DoSend(other_spawn_packet);
	}
}

void PacketHandler::HandleGetRoomList(SessionPtr session, const Common::CSGetRoomList& packet)
{
	auto room_list{ GET_SINGLE(RoomManager)->GetRoomList() };

	Common::SCGiveRoomList room_list_packet{
		static_cast<uint16>(room_list.size())
	};
	room_list_packet.size += static_cast<uint16>(sizeof(Common::RoomInfo) * room_list.size());

	// sendbuffer에 패킷과 방 리스트를 직접 직렬화해서 보냄.
	auto buffer{ Serializer::Serialize(room_list_packet, room_list) };
	session->DoSend(buffer);
}
