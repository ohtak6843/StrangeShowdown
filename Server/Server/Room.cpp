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
	auto id{ session->GetSessionID() };
	auto player{ GET_SINGLE(ObjectManager)->Pop<Player>() };
	
	// 플레이어 초기화 todo: 실패시 처리
	player->Init(shared_from_this(), session);
	session->SetPlayer(player);
	_players[id] = player;

	// 플레이어 입장 성공을 클라이언트에 알림
	Common::SCJoinRoom join_packet{ true, _hostID, id };
	session->DoSend(join_packet);

	// 다른 플레이어에게 스폰 패킷 전달
	Common::SCSpawnPlayer spawn_packet{
		id,
		player->GetPosition(),
		player->GetDirection(),
		player->GetType()
	};

	for (const auto& [other_id, other_player] : _players)
	{
		if (other_id == id)
		{
			continue;
		}

		// 다른 플레이어에게 스폰 패킷 전달
		other_player->GetOwnerSession()->DoSend(spawn_packet);

		Common::SCSpawnPlayer other_spawn_packet{
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
	// 예외 처리

	// 방이 로비인지	
	if (RoomState::LOBBY != _state)
	{
		return;
	}
	
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

	Broadcast(ready_packet, id);
}

void Room::HandleStart(const SessionPtr session)
{
	// 예외 처리

	// 방이 로비 상태인지 검사.
	if (RoomState::LOBBY != _state)
	{
		return;
	}

	// 플레이어가 방장인지 검사.
	if (session->GetSessionID() != _hostID)
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


	// 모든 플레이어에게 게임 시작 신호를 보냄.
	for (const auto& [id, player] : _players)
	{
		player->ChangeType(Common::PlayerType::Player);
		player->GetOwnerSession()->DoSend(Common::SCStartGame{ true });
	}

}

void Room::HandleChat(const SessionPtr session, const Common::CSChat& packet, const uint8* payload, const uint16 payload_size)
{

	auto id{ session->GetSessionID() };
	
	// message
	std::vector<uint8> additional_data(payload, payload + payload_size);
	additional_data.push_back('\0');

	// packet
	Common::SCChat chat_packet{
		id,
		static_cast<uint16>(payload_size + 1)
	};

	// 직렬화
	auto buffer{ Serializer::Serialize(chat_packet, additional_data) };

	// 모든 다른 플레이어에게 직렬화된 데이터 전송
	Broadcast(buffer, id);
}

void Room::HandleUseItem(const SessionPtr session, const Common::CSUseItem& packet)
{
	// 예외 처리
	if (RoomState::INGAME != _state)
	{
		return;
	}

	// 아이템 사용 가능 여부 검사 및 사용 처리
	auto player{ session->GetPlayer() };
	if (nullptr == player)
	{
		return;
	}
	if (false == player->TryConsumeItem(packet.itemType))
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
	player->ApplyItemEffect(packet, target);

	// debug: 아이템 패킷 종류 출력
	std::println("Player {} used item {} on target {}", session->GetSessionID(), static_cast<int>(packet.itemType), packet.targetID);
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