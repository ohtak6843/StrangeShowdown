#include "pch.h"
#include "RoomManager.h"

void RoomManager::Init()
{
	CreateRoom();
}

void RoomManager::AddPlayer(uint64 playerId, const std::shared_ptr<Player>& player)
{
	// 메모리 메니저에서 꺼내오자
	_players[playerId] = player;
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
}

void RoomManager::CreateRoom()
{
	// todo: 메모리 매니저에서 꺼내오자.
	// todo: 현재 room이 있는지 검사할 필요가 있음.
	// todo: 나중에 방 입장 코드도 필요
	_rooms[_roomCounter++] = std::make_shared<Room>();
}
