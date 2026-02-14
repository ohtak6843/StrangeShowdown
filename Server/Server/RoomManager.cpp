#include "pch.h"
#include "RoomManager.h"

void RoomManager::Init()
{
	CreateRoom();
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

void RoomManager::CreateRoom()
{
	// todo: 메모리 매니저에서 꺼내오자.
	// 현재 room이 있는지 검사할 필요가 있음.
	// 나중에 방 입장 코드도 필요

	auto room_id{ _roomCounter++ };
	_rooms[room_id] = std::make_shared<Room>();
	_rooms[room_id]->SetRoomID(room_id);
}

std::vector<Common::RoomInfo> RoomManager::GetRoomList()
{
	std::vector<Common::RoomInfo> room_list;
	for(auto& [room_id, room] : _rooms)
	{
		room_list.emplace_back(
			room_id,
			static_cast<uint8>(room->GetPlayers().size())
		);
	}
	return room_list;
}
