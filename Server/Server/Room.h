#pragma once

#include "Protocol.h"
#include "Job.h"

class Room
{
public:

	// --
	// job method
	// --

	// 밀려있는 job을 실행.
	void Update();

	// jobQueue에 LF로 job을 push
	void PushJob(Job& job);


	// --
	// handler method
	// --

	void HandleCreateRoom(const uint32 roomID, const Common::CSCreateRoom& packet);
	void HandleJoinRoom(const SessionPtr session, const Common::CSJoinRoom& packet);
	void HandleReady(const SessionPtr session, const Common::CSReady& packet);
	void HandleStart(const SessionPtr session);
	// void HandleChat(const SessionPtr session, const Common::CSChat& packet, const char* message);





	// --
	// other method
	// -- 

	void RemovePlayer(const uint64 playerId) { _players.erase(playerId); }
	void AddPlayer(const uint64 playerId, const std::shared_ptr<Player>& player)
	{
		_players[playerId] = player;
	}

	
	
	
	// --
	// getter and setter
	// --

	const std::unordered_map<uint64, std::shared_ptr<Player>>& GetPlayers() const
	{
		return _players;
	}

	bool HasPassword() const { return _hasPassword; }
	
	std::string GetPassword() const { return _password; }

	std::string GetName() const { return _name; }

	void SetRoomID(const uint32 roomID) { _roomID = roomID; }

private:

	// --
	// Job Quueue
	// --

	std::atomic<bool> _busy{ false };
	concurrency::concurrent_queue<Job> _jobQueue;


	// --
	// Content
	// --

	// 현재 방에 있는 플레이어 수
	std::unordered_map<uint64, std::shared_ptr<Player>> _players{};

	uint32 _roomID{};
	std::string _name{};
	bool _hasPassword{ false };
	std::string _password{};
	
	// 임시. 이후 enum으로 변경
	bool _inGame{ false };
};

