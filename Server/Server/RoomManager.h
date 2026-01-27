#pragma once
#include "Room.h"
#include "Player.h"

class RoomManager
{
	DECLARE_SINGLE(RoomManager)

public:
	void AddPlayer(uint64 playerId, const std::shared_ptr<Player>& player);

	std::shared_ptr<Player> GetPlayer(uint64 playerId);

	void RemovePlayer(uint64 playerId);

private:
	std::unordered_map<uint64, std::shared_ptr<Player>> _players{};
};

