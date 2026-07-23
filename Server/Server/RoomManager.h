#pragma once
#include "Room.h"
#include "Player.h"

class RoomManager
{
	DECLARE_SINGLE(RoomManager)

public:

	// --
	// Handle Packet method
	// --

	// todo: thread unsafe
	void HandleCreateRoom(SessionPtr session, const Common::CSCreateRoom& packet);
	void HandleJoinRoom(SessionPtr session, const Common::CSJoinRoom& packet);
	void HandleGetRoomList(SessionPtr session, const Common::CSGetRoomList& packet);

	
	// todo: 현재 thread unsafe!!!!
private:
	std::vector<Common::RoomInfo> GetRoomList();



	// --
	// Member variables
	// --
private:
	// lock 필요할듯..
	// room id 부여
	std::atomic<uint32> _roomCounter{ 0 };
	
	std::unordered_map<uint32, std::shared_ptr<Room>> _rooms{};
	std::unordered_map<uint64, std::shared_ptr<Player>> _players{};
};

