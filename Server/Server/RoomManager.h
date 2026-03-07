#pragma once
#include "Room.h"
#include "Player.h"

class RoomManager
{
	DECLARE_SINGLE(RoomManager)

public:
	
	void Init();

	// todo: 현재 thread unsafe!!!!
	// 현재는 임시로 
	void AddPlayer(uint64 playerId, const std::shared_ptr<Player> player);

	std::shared_ptr<Player> GetPlayer(uint64 playerId);

	// todo: 현재 thread unsafe!!!!
	void RemovePlayer(uint64 playerId);

	// todo: 현재 thread unsafe!!!!
	// @Param: room_id 방 번호 반환용 아웃 파라미터
	// @Return: 방 제작 성공 여부
	bool CreateRoom(OUT uint32& room_id);

	void JoinRoom(SessionPtr session, uint32 room_id);

	// todo: 현재 thread unsafe!!!!
	std::vector<Common::RoomInfo> GetRoomList();

private:
	// lock 필요할듯..
	
	// room id 부여
	std::atomic<uint32> _roomCounter{ 0 };
	
	std::unordered_map<uint32, std::shared_ptr<Room>> _rooms{};
	std::unordered_map<uint64, std::shared_ptr<Player>> _players{};
};

