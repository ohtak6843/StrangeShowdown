#include "pch.h"
#include "PacketHandler.h"
#include "RoomManager.h"
#include "IOCP.h"
#include "Serializer.h"

std::unordered_map<packet::Type, PacketHandler::HandlerFunc> PacketHandler::_handlerMap;

void PacketHandler::Init()
{
	// 핸들러 등록
	RegisterHandler<packet::CSLogin>(
		packet::Type::CS_LOGIN,
		[](SessionPtr session, const auto& pkt) {
			HandleLogin(session, pkt);
		}
	);

	RegisterHandler<packet::CSMovePlayer>(
		packet::Type::CS_MOVE_PLAYER,
		[](SessionPtr session, const auto& pkt) {
			HandleMovePlayer(session, pkt);
		}
	);
}

void PacketHandler::HandlePacket(SessionPtr session, const RecvBuffer& buffer)
{
	if (nullptr == session)
	{
		return;
	}

	auto type{ static_cast<packet::Type>(buffer[1]) };
	auto iter{ _handlerMap.find(type) };
	if (_handlerMap.end() != iter)
	{
		auto& handler_func{ iter->second };
		handler_func(session, buffer);
	}
}

void PacketHandler::HandleMovePlayer(SessionPtr session, const packet::CSMovePlayer& packet)
{
	auto id{ session->GetSessionID() };
	auto player{ GET_SINGLE(RoomManager)->GetPlayer(id) };

	player->SetPosition(packet.pos);
	player->SetDirection(packet.dir);

	// 다른 플레이어에게도 전파
	auto player_map{ GET_SINGLE(RoomManager)->GetRoom(id)->GetPlayers() };

	for (const auto& [other_id, other_player] : player_map)
	{
		if (other_id == id)
		{
			continue;
		}

		// 다른 플레이어에게 내 위치 전달
		packet::SCMoveObject move_object_packet{
			id,
			player->GetPosition(),
			player->GetDirection()
		};
		other_player->GetOwnerSession()->DoSend(move_object_packet);
	}
}

void PacketHandler::HandleLogin(SessionPtr session, const packet::CSLogin& packet)
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
		packet::SCSpawnObject move_object_packet{
			id,
			player->GetPosition(),
			player->GetDirection(),
		};
		other_player->GetOwnerSession()->DoSend(move_object_packet);

		// 현재 클라이언트에 기존 플레이어 정보 전달
		packet::SCSpawnObject other_spawn_packet{
			other_id,
			other_player->GetPosition(),
			other_player->GetDirection()
		};
		session->DoSend(other_spawn_packet);
	}
}
