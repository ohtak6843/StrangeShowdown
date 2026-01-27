#include "pch.h"
#include "PacketHandler.h"
#include "RoomManager.h"
#include "IOCP.h"

//void PacketHandler::HandlePacket(std::shared_ptr<Session> session, char* buffer, int len) {
//    PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
//    uint16_t pktId = header->id;
//
//    // 등록된 핸들러가 있다면 실행
//    if (_handlerTable[pktId]) {
//        _handlerTable[pktId](session, buffer);
//    }
//}


//void PacketHandler::Handle_CS_MOVE(std::shared_ptr<Session> session, void* pkt) {
//    // 1. 패킷 캐스팅
//    PKT_CS_MOVE* movePkt = static_cast<PKT_CS_MOVE*>(pkt);
//
//    // 2. 세션이 속한 방 가져오기
//    auto room = session->GetRoom();
//    if (!room) return;
//
//    // 3. Job 생성 (람다 캡처로 데이터 복사)
//    // 데이터 보존을 위해 movePkt의 내용을 값 복사하거나 별도 보관해야 함
//    auto job = new Job([session, room, x = movePkt->x, y = movePkt->y]() {
//        // [여기는 WorkerThread가 실행함]
//        room->HandleMove(session, x, y);
//        });
//
//    // 4. 방 큐에 삽입
//    room->PushJob(job);
//}

void PacketHandler::HandlePacket(std::shared_ptr<Session> session, char* buffer, int len)
{
	// todo: 패킷 핸들러 구현

	// todo 이걸 switch-case 대신 함수 포인터 테이블로 바꾸기
	packet::Type packet_id{ static_cast<uint8>(buffer[1]) };
	switch (packet_id)
	{
		case packet::Type::CS_LOGIN:
		{
			// HandleCSLogin(session, buffer);
			std::println("Login Successed for session id: {}", session->GetSessionID());
			break;
		}

		case packet::Type::CS_MOVE_PLAYER:
		{
			PacketHandler::HandleCSMovePlayer(session, buffer);
			break;
		}
	}
}

void PacketHandler::HandleCSMovePlayer(std::shared_ptr<Session> session, void* packet)
{
	auto* move_packet{ static_cast<packet::CSMovePlayer*>(packet) };

	auto id{ session->GetSessionID() };
	auto player{ GET_SINGLE(RoomManager)->GetPlayer(id) };

	player->_position = move_packet->pos;
	player->_direction = move_packet->dir;

	// todo: 변경
	// 다른 플레이어에게도 전파
	auto begin_iter{ GET_SINGLE(IOCP)->_sessionHash.begin() };
	auto end_iter{ GET_SINGLE(IOCP)->_sessionHash.end() };
	for (; begin_iter != end_iter; ++begin_iter)
	{
		auto other_session{ begin_iter->second };
		if (other_session->GetSessionID() == id)
		{
			continue;
		}

		// todo: 이부분을 수정
		packet::SCMoveObject move_object_packet{
			id,
			player->_position,
			player->_direction
		};
		std::vector<char> buffer(move_object_packet.size);
		std::memcpy(buffer.data(), &move_object_packet, move_object_packet.size);
		other_session->DoSend(buffer);
	}
}