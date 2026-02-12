#pragma once
#include "Room.h"
#include "Player.h"

class RoomManager
{
	DECLARE_SINGLE(RoomManager)

public:
	
	void Init();

	// todo: 현재 thread unsafe!!!!
	void AddPlayer(uint64 playerId, const std::shared_ptr<Player>& player);

	std::shared_ptr<Player> GetPlayer(uint64 playerId);

	// todo: 현재 thread unsafe!!!!
	void RemovePlayer(uint64 playerId);

	// todo: 현재 thread unsafe!!!!
	// todo: 임시로 0번 방을 반환.
	// [session_id, room_id] map이 필요
	void CreateRoom();
	std::shared_ptr<Room> GetRoom(uint64 session_id)
	{
		return _rooms[0];
	}

private:
	// lock 필요할듯..
	
	// room id 부여
	std::atomic<uint32> _roomCounter{ 0 };
	
	std::unordered_map<uint32, std::shared_ptr<Room>> _rooms{};
	std::unordered_map<uint64, std::shared_ptr<Player>> _players{};
};

