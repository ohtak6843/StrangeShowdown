#include "pch.h"
#include "RoomManager.h"

void RoomManager::AddPlayer(uint64 playerId, const std::shared_ptr<Player>& player)
{
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
