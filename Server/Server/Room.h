#pragma once

#include "protocol.h"
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
	// content method
	// -- 

	void Init(const uint32 roomID, const Common::CSCreateRoom& packet);

	bool JoinRoom(const Common::CS);

	void AddPlayer(uint64 playerId, const std::shared_ptr<Player>& player)
	{
		_players[playerId] = player;
	}

	void RemovePlayer(uint64 playerId)
	{
		_players.erase(playerId);
	}

	// --
	// getter and setter
	// --

	const std::unordered_map<uint64, std::shared_ptr<Player>>& GetPlayers() const
	{
		return _players;
	}

	void SetRoomID(const uint32 roomID)
	{
		_roomID = roomID;
	}

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
};

