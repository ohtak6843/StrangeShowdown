#pragma once

#include "Protocol.h"
#include "Job.h"

enum class RoomState
{
	WAITING,
	LOBBY,
	INGAME,
};

class Room
{
	// --
	// job method
	// --
public:
	// 밀려있는 job을 실행.
	void Update();

	// jobQueue에 LF로 job을 push
	void PushJob(Job& job);


	// --
	// handler method
	// --
public:
	// static
	
	void HandleCreateRoom(const uint32 roomID, const SessionPtr session, const Common::CSCreateRoom& packet);
	void HandleJoinRoom(const SessionPtr session, const Common::CSJoinRoom& packet);
	void HandleReady(const SessionPtr session, const Common::CSReady& packet);
	void HandleStart(const SessionPtr session);
	void HandleUseItem(const SessionPtr session, const Common::CSUseItem& packet);

	// dynamic 
	void HandleChat(const SessionPtr session, const Common::CSChat& packet, const uint8* payload, const uint16 payload_size);



	// --
	// item method
	// --
public:
	bool UseGun(const PlayerPtr target);




	// --
	// network method
	// -- 
public:
	void RemovePlayer(const uint64 playerId) { _players.erase(playerId); }
	void AddPlayer(const uint64 playerId, const std::shared_ptr<Player>& player) { _players[playerId] = player; }
	
	template<typename T>
	void Broadcast(const T& packet, const uint64 exclude_id = 0)
	{
		for (const auto& [other_id, other_player] : _players)
		{
			if (other_id == exclude_id)
			{
				continue;
			}
			auto session{ other_player->GetOwnerSession() };
			if (nullptr != session)
			{
				session->DoSend(packet);
			}
		}
	}
	
	// --
	// getter and setter
	// --
public:
	const std::unordered_map<uint64, std::shared_ptr<Player>>& GetPlayers() const { return _players; }

	bool HasPassword() const { return _hasPassword; }
	std::string GetPassword() const { return _password; }

	std::string GetName() const { return _name; }

	void SetRoomID(const uint32 roomID) { _roomID = roomID; }


	// --
	// Job Quueue variables
	// --
private:
	std::atomic<bool> _busy{ false };
	concurrency::concurrent_queue<Job> _jobQueue;


	// --
	// Content
	// --
private:
	// 현재 방에 있는 플레이어 수
	std::unordered_map<uint64, std::shared_ptr<Player>> _players{};

	uint32 _roomID{};
	std::string _name{};
	bool _hasPassword{ false };
	std::string _password{};
	uint64 _hostID{};
	
	// 임시. 이후 enum으로 변경
	RoomState _state{ RoomState::WAITING };
};