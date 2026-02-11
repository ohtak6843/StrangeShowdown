#pragma once
#include "Session.h"
#include "Serializer.h"

//class PacketHandler {
//public:
//    // 전역 초기화 (함수 포인터 테이블 등록 등)
//    // static void Init();
//
//    // [핵심] 재조립된 패킷을 보고 알맞은 핸들러 함수를 호출
//    static void HandlePacket(std::shared_ptr<Session> session, char* buffer, int len);
//
//private:
//    //// 각 패킷 ID별 실제 처리 로직 (Job 생성기)
//    //static void Handle_CS_MOVE(std::shared_ptr<Session> session, void* pkt);
//    //static void Handle_CS_CHAT(std::shared_ptr<Session> session, void* pkt);
//
//private:
//    // 함수 포인터 배열이나 맵을 사용해 switch-case를 제거하면 성능이 더 좋습니다.
//    using HandlerFunc = std::function<void(std::shared_ptr<Session>, void*)>;
//    static HandlerFunc _handlerTable[UINT16_MAX];
//};

class PacketHandler
{
public:
	static void Init();

	static void HandlePacket(SessionPtr session, const RecvBuffer& data);

private:
    static void HandleMovePlayer(SessionPtr session, const packet::CSMovePlayer& packet);
	static void HandleLogin(SessionPtr session, const packet::CSLogin& packet);

	template <typename T>
	static void RegisterHandler(packet::Type type, std::function<void(SessionPtr, const T&)> logic_func)
	{
		_handlerMap.emplace(type, [logic_func](SessionPtr session, const RecvBuffer& Data) {
			T Pkt{ Serializer::Deserialize<T>(Data) };
			logic_func(session, Pkt);
			});
	}


	using HandlerFunc = std::function<void(std::shared_ptr<Session>, const RecvBuffer&)>;
	static std::unordered_map<packet::Type, HandlerFunc> _handlerMap;

};