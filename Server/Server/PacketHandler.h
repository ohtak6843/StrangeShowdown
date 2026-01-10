#pragma once
#include "Session.h"

class PacketHandler {
public:
    // 전역 초기화 (함수 포인터 테이블 등록 등)
    // static void Init();

    // [핵심] 재조립된 패킷을 보고 알맞은 핸들러 함수를 호출
    static void HandlePacket(std::shared_ptr<Session> session, char* buffer, int len);

private:
    //// 각 패킷 ID별 실제 처리 로직 (Job 생성기)
    //static void Handle_CS_MOVE(std::shared_ptr<Session> session, void* pkt);
    //static void Handle_CS_CHAT(std::shared_ptr<Session> session, void* pkt);

private:
    // 함수 포인터 배열이나 맵을 사용해 switch-case를 제거하면 성능이 더 좋습니다.
    using HandlerFunc = std::function<void(std::shared_ptr<Session>, void*)>;
    static HandlerFunc _handlerTable[UINT16_MAX];
};