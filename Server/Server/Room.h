#pragma once

#include "Job.h"

class Room
{
public:

	// 밀려있는 job을 실행.
	void Update();

	// jobQueue에 LF로 job을 push
	void PushJob(Job& job);

	void AddPlayer(uint64 playerId, const std::shared_ptr<Player>& player)
	{
		_players[playerId] = player;
	}

	void RemovePlayer(uint64 playerId)
	{
		_players.erase(playerId);
	}

	// getter and setter
	const std::unordered_map<uint64, std::shared_ptr<Player>>& GetPlayers() const
	{
		return _players;
	}

	void SetRoomID(const uint32 roomID)
	{
		_roomID = roomID;
	}

private:
	// Job Quueue
	std::atomic<bool> _busy{ false };
	concurrency::concurrent_queue<Job> _jobQueue;

	// 현재 방에 있는 플레이어 수
	std::unordered_map<uint64, std::shared_ptr<Player>> _players{};

	// 현재 방 번호
	uint32 _roomID{};
};

