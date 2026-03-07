#include "pch.h"
#include "RoomManager.h"
#include "ObjectManager.h"

void RoomManager::Init()
{
}

void RoomManager::AddPlayer(uint64 playerId, const std::shared_ptr<Player> player)
{
	// 메모리 메니저에서 꺼내오자
	_players[playerId] = player;

	// 추가로 일단 임시로 0번 방에 추가
	_rooms[0]->AddPlayer(playerId, player);
}

std::shared_ptr<Player> RoomManager::GetPlayer(uint64 playerId)
{
	auto it = _players.find(playerId);
	if (it != _players.end())
	{
		return it->second;
	}
	return nullptr;
}
void RoomManager::RemovePlayer(uint64 playerId)
{
	_players.erase(playerId);
	_rooms[0]->RemovePlayer(playerId);
}

bool RoomManager::CreateRoom(OUT uint32& room_id)
{
	// todo: 메모리 매니저에서 꺼내오자.
	// 현재 room이 있는지 검사할 필요가 있음.
	// 나중에 방 입장 코드도 필요
	// 나중에 방 허용량이 다 찼을 경우엔 실패 반환

	auto id{ _roomCounter++ };
	_rooms[id] = GET_SINGLE(ObjectManager)->Pop<Room>();
	_rooms[id]->SetRoomID(id);
	return true;
}

void RoomManager::JoinRoom(SessionPtr session, uint32 room_id)
{
	const auto res{ _rooms.find(room_id) };
	const auto id{ session->GetSessionID() };
	if (res == _rooms.end())
	{
		Common::SCJoinRoom join_packet{ false };
		session->DoSend(join_packet);
		return;
	}

	auto room{ res->second };

	// 플레이어 입장 성공을 클라이언트에 알림
	
	session->SetRoom(room);

	Common::SCJoinRoom join_packet{ true };
	session->DoSend(join_packet);
	
	// 플레이어 생성 및 방에 추가
	auto player{ GET_SINGLE(ObjectManager)->Pop<Player>() };
	player->SetOwnerSession(session);
	room->AddPlayer(id, player);
	
	auto& player_map{ room->GetPlayers() };
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

	// todo: 나중에 cas로 변경 필요 한지 확인
	session->SetIOState(IOState::INGAME);
}

std::vector<Common::RoomInfo> RoomManager::GetRoomList()
{
	std::vector<Common::RoomInfo> room_list;
	for (auto& [room_id, room] : _rooms)
	{
		room_list.emplace_back(
			room_id,
			static_cast<uint8>(room->GetPlayers().size())
		);
	}
	return room_list;
}
