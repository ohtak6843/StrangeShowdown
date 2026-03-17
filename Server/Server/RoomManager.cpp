#include "pch.h"
#include "RoomManager.h"
#include "ObjectManager.h"



void RoomManager::HandleCreateRoom(SessionPtr session, const Common::CSCreateRoom& packet)
{
	// todo: 메모리 매니저에서 꺼내오자.
	// 현재 room이 있는지 검사할 필요가 있음.
	// 나중에 방 입장 코드도 필요
	// 나중에 방 허용량이 다 찼을 경우엔 실패 반환

	// 방 생성 및 초기화
	auto id{ _roomCounter++ };
	_rooms[id] = GET_SINGLE(ObjectManager)->Pop<Room>();
	_rooms[id]->Init(id, packet);

	// 방을 성공적으로 만들었으면 패킷을 보내준다
	Common::SCCreateRoom room_packet{ true };
	session->DoSend(room_packet);

	// HandleJoinRoom을 통해 방에 입장한다.
	HandleJoinRoom(session, Common::CSJoinRoom{ id, packet.password });
}

void RoomManager::HandleJoinRoom(SessionPtr session, const Common::CSJoinRoom& packet)
{
	const auto res{ _rooms.find(packet.roomID) };
	const auto id{ session->GetSessionID() };
	if (res == _rooms.end())
	{
		// 플레이어 입장 실패을 클라이언트에 알림
		Common::SCJoinRoom join_packet{ false };
		session->DoSend(join_packet);
		return;
	}

	auto room{ res->second };

	// 플레이어 입장 성공을 클라이언트에 알림
	session->SetRoom(room);
	Common::SCJoinRoom join_packet{ true };
	session->DoSend(join_packet);
	
	// 플레이어 생성 및 방에 추가
	auto player{ GET_SINGLE(ObjectManager)->Pop<Player>() };
	player->SetOwnerSession(session);
	room->AddPlayer(id, player);
	
	// 패킷 처리
	auto& player_map{ room->GetPlayers() };
	for (const auto& [other_id, other_player] : player_map)
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

std::vector<Common::RoomInfo> RoomManager::GetRoomList()
{
	std::vector<Common::RoomInfo> room_list;
	for (auto& [room_id, room] : _rooms)
	{
		room_list.emplace_back(
			room_id,
			static_cast<uint8>(room->GetPlayers().size())
		);
	}
	return room_list;
}
