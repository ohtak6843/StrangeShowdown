#include "pch.h"
#include "PacketHandler.h"

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
	std::println("HandlePacket called with packet length: {}", len);

	// 역직렬화 후 패킷 만들고
}
