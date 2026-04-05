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
	_hostID = session->GetSessionID();
}

void Room::HandleJoinRoom(const SessionPtr session, const Common::CSJoinRoom& packet)
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
	session->SetPlayer(player);
	_players[id] = player;

	Common::SCSpawnObject move_object_packet{
		id,
		player->GetPosition(),
		player->GetDirection(),
	};

	// 스폰 패킷 전달
	for (const auto& [other_id, other_player] : _players)
	{
		if (other_id == id)
		{
			continue;
		}

		// 다른 플레이어에게 내 위치 전달
		other_player->GetOwnerSession()->DoSend(move_object_packet);

		Common::SCSpawnObject other_spawn_packet{
			other_id,
			other_player->GetPosition(),
			other_player->GetDirection()
		};
		// 현재 클라이언트에 기존 플레이어 정보 전달
		session->DoSend(other_spawn_packet);
	}
}

void Room::HandleReady(const SessionPtr session, const Common::CSReady& packet)
{
	auto player{ session->GetPlayer() };
	if (player == nullptr)
	{
		return;
	}

	// 플레이어의 준비 상태 업데이트
	player->SetReady(packet.ready);

	// 모든 플레이어에게 해당 플레이어의 준비 상태 전달
	// todo: 나중에 session id대신 player id로 변경할 필요 있음
	auto id{ session->GetSessionID() };
	Common::SCReady ready_packet{
		id,
		packet.ready
	};

	for (const auto& [other_id, other_player] : _players)
	{
		if (other_id == id)
		{
			continue;
		}
		other_player->GetOwnerSession()->DoSend(ready_packet);
	}
}

void Room::HandleStart(const SessionPtr session)
{
	// todo: 플레이어가 방장인지 검사.
	if (session->GetSessionID() != _hostID)
	{
		return;
	}

	// 모든 플레이어가 준비중인지 검사.
	for (const auto& [id, player] : _players)
	{
		// 방장은 시작을 보냄.
		if (id == _hostID)
		{
			continue;
		}
		if (!player->IsReady())
		{
			// 시작 실패 패킷을 보냄.
			session->DoSend(Common::SCStartGame{ false });
			return;
		}
	}

	// 게임 시작.
	// todo: 게임 시작 하기 전 클라이언트 로딩을 기다려야 함.
	_inGame = true;


	// 모든 플레이어에게 게임 시작 신호를 보냄.
	for (const auto& [id, player] : _players)
	{
		player->GetOwnerSession()->DoSend(Common::SCStartGame{ true });
	}

}

void Room::HandleChat(const SessionPtr session, const Common::CSChat& packet, const uint8* payload, const uint16 payload_size)
{

	auto id{ session->GetSessionID() };
	
	// message
	std::vector<uint8> additional_data(payload, payload + payload_size);

	// packet
	Common::SCChat chat_packet{
		id,
		payload_size
	};

	// 직렬화
	auto buffer{ Serializer::Serialize(chat_packet, additional_data) };

	// 모든 다른 플레이어에게 직렬화된 데이터 전송
	for (const auto& [other_id, other_player] : _players)
	{
		if (other_id == id)
		{
			continue;
		}
		other_player->GetOwnerSession()->DoSend(buffer);
	}
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