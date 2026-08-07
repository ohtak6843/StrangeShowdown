#include "pch.h"
#include "Room.h"
#include "ObjectManager.h"
#include "Session.h"
#include "Player.h"

void Room::PushJob(Job& job)
{
	_jobQueue.push(job);
}

void Room::HandleCreateRoom(const uint32 roomID, const SessionPtr session, const Common::CSCreateRoom& packet)
{
	_roomID = roomID;
	_name = packet.name;
	_hasPassword = packet.hasPassword;
	_password = packet.password;
	_hostID = 0;
	_state = RoomState::LOBBY;
}

void Room::HandleJoinRoom(const SessionPtr session, const Common::CSJoinRoom& packet)
{
	// 예외 처리
	const bool is_lobby{ RoomState::LOBBY != _state };
	const bool is_wrong_password{ _hasPassword && _password != packet.password };
	const bool is_room_full{ _players.size() >= Common::MaxPlayerCount };

	if (is_lobby || is_wrong_password || is_room_full)
	{
		// 입장 실패를 클라이언트에 알림
		Common::SCJoinRoom join_packet{ false };
		session->DoSend(join_packet);
		return;
	}


	// 플레이어 생성
	auto my_id{ _currentId++ };
	auto my_player{ GET_SINGLE(ObjectManager)->Pop<Player>() };


	// 방장이 없는 경우 새로 들어온 플레이어를 방장으로 설정
	if (0 == _hostID)
	{
		_hostID = my_id;
	}
	
	// 플레이어 초기화 todo: 실패시 처리
	my_player->Init(shared_from_this(), session, my_id);
	session->SetPlayer(my_player);
	_players[my_id] = my_player;

	// 플레이어 입장 성공을 클라이언트에 알림
	Common::SCJoinRoom join_packet{ true, _hostID, my_id };
	session->DoSend(join_packet);

	// 다른 플레이어에게 스폰 패킷 전달
	Common::SCSpawnPlayer spawn_packet{
		my_id,
		my_player->GetPosition(),
		my_player->GetDirection(),
		my_player->GetType()
	};

	for (const auto& [other_id, other_player] : _players)
	{

		// 모든 플레이어에게 새로 들어온 플레이어의 스폰 패킷 전달

		other_player->GetOwnerSession()->DoSend(spawn_packet);
		if (other_id ==	my_id)
		{
			continue;
		}

		// 현재 클라이언트에 기존 플레이어 정보 전달
		Common::SCSpawnPlayer other_spawn_packet{
			other_id,
			other_player->GetPosition(),
			other_player->GetDirection(),
			other_player->GetType()
		};


		session->DoSend(other_spawn_packet);
	}
}

void Room::HandleReady(const SessionPtr session, const Common::CSReady& packet)
{
	// 예외 처리

	// 방이 로비인지	
	if (RoomState::LOBBY != _state)
	{
		return;
	}
	
	auto my_player{ session->GetPlayer() };
	if (my_player == nullptr)
	{
		return;
	}

	// 플레이어의 준비 상태 업데이트
	my_player->SetReady(packet.ready);

	// 모든 플레이어에게 해당 플레이어의 준비 상태 전달
	auto my_id{ my_player->GetObjectId() };
	Common::SCReady ready_packet{
		my_id,
		packet.ready
	};

	Broadcast(ready_packet, my_id);
}

void Room::HandleStart(const SessionPtr session)
{
	// 예외 처리
	auto my_player{ session->GetPlayer() };
	if (my_player == nullptr)
	{
		return;
	}
	auto my_id{ my_player->GetObjectId() };

	// 방이 로비 상태인지 검사.
	if (RoomState::LOBBY != _state)
	{
		return;
	}

	// 플레이어가 방장인지 검사.
	if (my_id != _hostID)
	{
		return;
	}

	// 모든 플레이어가 준비중인지 검사.
	for (const auto& [id, player] : _players)
	{
		// 방장은 시작을 보내므로 검사에서 제외.
		if (id == _hostID)
		{
			continue;
		}

		// 준비가 안된 플레이어가 있을 경우
		if (false == player->GetReady())
		{
			// 시작 실패 패킷을 보냄.
			session->DoSend(Common::SCStartGame{ false });
			return;
		}
	}

	// 게임 시작.
	
	// 플레이어를 인게임 플레이어로 변경
	// todo: 게임 시작 하기 전 클라이언트 로딩을 기다려야 함.
	_state = RoomState::INGAME;
	_currentTurn = 0;

	for (const auto& [id, player] : _players)
	{
		player->ChangePlayerType(Common::PlayerType::Player);
	}

	// 모든 플레이어에게 게임 시작 신호를 보냄.
	Common::SCStartGame start_packet{ true };
	for (const auto& [id, player] : _players)
	{
		// todo: weak ptr화
		auto session{ player->GetOwnerSession() };
		if (nullptr != session)
		{
			session->DoSend(start_packet);
		}

		// 현재 세션에 모든 방의 플레이어 소환 패킷을 보냄
		for (const auto& [other_id, other_player] : _players)
		{
			Common::SCSpawnPlayer spawn_packet{
				other_id,
				other_player->GetPosition(),
				other_player->GetDirection(),
				other_player->GetType()
			};

			session->DoSend(spawn_packet);
		}
		
	}

	OnTurnEnded();

}

void Room::HandleChat(const SessionPtr session, const Common::CSChat& packet, const uint8* payload, const uint16 payload_size)
{
	auto my_player{ session->GetPlayer() };
	if (my_player == nullptr)
	{
		return;
	}
	auto my_id{ my_player->GetObjectId() };
	
	// message
	std::vector<uint8> additional_data(payload, payload + payload_size);
	additional_data.push_back('\0');

	// packet
	Common::SCChat chat_packet{
		my_id,
		static_cast<uint16>(payload_size + 1)
	};

	// 직렬화
	auto buffer{ Serializer::Serialize(chat_packet, additional_data) };

	// 모든 다른 플레이어에게 직렬화된 데이터 전송
	Broadcast(buffer, my_id);
}

void Room::ChangeRoomState(const RoomState state)
{

}

void Room::OnTurnEnded()
{
	// 턴 종료시 처리
	++_currentTurn;

	// 지연 작업 추가
	Job timer_job{ [this]() {
		OnTurnEnded();
	} };
	PushDelayedJob(timer_job, _turnTime);

	// 모든 플레이어에게 턴 종료 패킷 전송
	Common::SCSetTurn start_packet{ _currentTurn, _turnTime };
	for (const auto& [id, player] : _players)
	{
		// todo: weak ptr화
		auto session{ player->GetOwnerSession() };
		if (nullptr != session)
		{
			session->DoSend(start_packet);
		}
	}

#ifdef DEBUG
	std::println("Turn {} ended. Next turn will start in {} seconds.", _currentTurn, _turnTime);
#endif // DEBUG

}

void Room::HandleUseItem(const SessionPtr session, const Common::CSUseItem& packet)
{
	// 예외 처리
	if (RoomState::INGAME != _state)
	{
		return;
	}

	auto my_player{ session->GetPlayer() };
	if (nullptr == my_player)
	{
		return;
	}
	auto my_id{ my_player->GetObjectId() };

	// 아이템 사용 가능 여부 검사 및 사용 처리
	if (false == my_player->TryConsumeItem(packet.itemType))
	{
		return;
	}

	// 타겟 대상
	PlayerPtr target{};
	auto target_iter{ _players.find(packet.targetID) };
	if (target_iter == _players.end())
	{
		target = nullptr;
	}
	else
	{
		target = target_iter->second;
	}

	// 효과 적용 및 패킷 전송
	my_player->ApplyItemEffect(packet, target);

#ifdef DEBUG
	std::println("Player {} used item {} on target {}", my_id, static_cast<int>(packet.itemType), packet.targetID);
#endif
}


void Room::Update()
{
	bool expected{ false };

	// job 처리는 하나의 스레드만 들어가도록 보장
	if (_busy.compare_exchange_strong(expected, true))
	{
		auto now = std::chrono::steady_clock::now();

		// 1. _delayedJobQueue에서 처리 시간이 된 작업들 옮기기
		{
			std::lock_guard<std::mutex> lock{ _delayedJobMutex };
			while (!_delayedJobQueue.empty())
			{
				const DelayedJob& topJob{ _delayedJobQueue.top() };
				if (topJob.executeTime <= now)
				{
					// 실행 시간이 지났으면 _jobQueue로 옮기고 우선순위 큐에서 제거
					_jobQueue.push(topJob.job);
					_delayedJobQueue.pop();
				}
				else
				{
					// 가장 앞에 있는 작업의 시간이 아직 안 되었다면, 뒤의 작업들도 아직 안 된 것이 보장됨
					break;
				}
			}
		} // _delayedJobMutex lock 해제

		// 2. 모든 큐 비우기 (일반 큐에 들어있던 작업 + 방금 넘어온 지연 작업)
		Job job;
		while (true == _jobQueue.try_pop(job))
		{
			job.Execute();
		}

		_busy = false;
	}
}

// 외부(멀티스레드)에서 지연된 작업을 넣을 때 사용하는 함수
void Room::PushDelayedJob(Job& job, const float delayTime)
{
	auto delayDuration{ std::chrono::duration_cast<std::chrono::steady_clock::duration>(
		std::chrono::duration<float>(delayTime)
	) };

	auto executeTime{ std::chrono::steady_clock::now() + delayDuration };
	DelayedJob delayedJob{ executeTime, job };

	// Multi-thread push 상황에 대한 락
	{
		std::lock_guard<std::mutex> lock{ _delayedJobMutex };
		_delayedJobQueue.push(delayedJob);
	}
}