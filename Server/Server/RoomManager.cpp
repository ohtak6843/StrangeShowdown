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
	_rooms[id]->HandleCreateRoom(id, session, packet);

	// 방을 성공적으로 만들었으면 패킷을 보내준다
	Common::SCCreateRoom room_packet{ true };
	session->DoSend(room_packet);

	// HandleJoinRoom을 통해 방에 입장한다.
	HandleJoinRoom(session, Common::CSJoinRoom{ id, packet.password });
}

void RoomManager::HandleJoinRoom(SessionPtr session, const Common::CSJoinRoom& packet)
{
	// 패킷에서 받은 id가 존재하는지 검사
	const auto res{ _rooms.find(packet.roomID) };
	if (res == _rooms.end())
	{
		// 플레이어 입장 실패을 클라이언트에 알림
		Common::SCJoinRoom join_packet{ false };
		session->DoSend(join_packet);
		return;
	}

	// 방이 존재하면 입장 시도
	// todo: cas 실행
	session->SetIOState(IOState::INGAME);

	auto room{ res->second };
	session->SetRoom(room);

	// todo: 나중에 cas로 변경 필요 한지 확인
	room->HandleJoinRoom(session, packet);
}

std::vector<Common::RoomInfo> RoomManager::GetRoomList()
{
	std::vector<Common::RoomInfo> room_list;
	for (auto& [room_id, room] : _rooms)
	{
		Common::RoomInfo info{};
		info.roomId = room_id;
		info.currentPlayerCount = static_cast<uint8>(room->GetPlayers().size());
		info.hasPassword = room->HasPassword();
		strcpy_s(info.name, room->GetName().c_str());
		room_list.push_back(info);
	}
	return room_list;
}
