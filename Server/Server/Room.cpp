#include "pch.h"
#include "Room.h"
#include "ObjectManager.h"
#include "Session.h"
#include "Player.h"

void Room::PushJob(Job& job)
{
	_jobQueue.push(job);
}

void Room::Init(const uint32 roomID, const Common::CSCreateRoom& packet)
{
	_roomID = roomID;
	_name = packet.name;
	_hasPassword = packet.hasPassword;
	_password = packet.password;
}

void Room::JoinRoom(SessionPtr session, const Common::CSJoinRoom& packet)
{
	// 입력한 비밀번호 비교
	if (true == _hasPassword && _password != packet.password)
	{
		// 플레이어 입장 실패을 클라이언트에 알림
		Common::SCJoinRoom join_packet{ false };
		session->DoSend(join_packet);
		return;
	}

	// todo: 방 허용량이 다 찼을 경우엔 실패 반환


	// 플레이어 입장 성공을 클라이언트에 알림
	Common::SCJoinRoom join_packet{ true };
	session->DoSend(join_packet);

	// 플레이어 생성 및 방에 추가
	auto id{ session->GetSessionID() };
	auto player{ GET_SINGLE(ObjectManager)->Pop<Player>() };
	player->SetOwnerSession(session);
	_players[id] = player;

	// 스폰 패킷 전달
	for (const auto& [other_id, other_player] : _players)
	{
		if (other_id == id)
		{
			continue;
		}

		// 다른 플레이어에게 내 위치 전달
		Common::SCSpawnObject move_object_packet{
			id,
			player->GetPosition(),
			player->GetDirection(),
		};
		other_player->GetOwnerSession()->DoSend(move_object_packet);

		// 현재 클라이언트에 기존 플레이어 정보 전달
		Common::SCSpawnObject other_spawn_packet{
			other_id,
			other_player->GetPosition(),
			other_player->GetDirection()
		};
		session->DoSend(other_spawn_packet);
	}

	// todo: 나중에 cas로 변경 필요 한지 확인
	session->SetIOState(IOState::INGAME);
}


void Room::Update()
{
	bool expected{ false };

	// job 처리는 하나의 스레드만 들어가도록 보장
	if (_busy.compare_exchange_strong(expected, true))
	{
		// 모든 큐 비우기
		Job job;
		while (true == _jobQueue.try_pop(job))
		{
			job.Execute();
		}

		_busy = false;
	}
}