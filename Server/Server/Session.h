#pragma once

#include "OverlappedEx.h"
#include "Serializer.h"

class Room;

class Session : public std::enable_shared_from_this<Session>
{
public:

	Session();

	~Session() {}

	// 소켓 및 id 초기화.
	void Init(const SOCKET client_socket, const uint64 id);

	// reference counting 및 recv 시작
	void Start();


	// recv 시작
	void DoRecv();

	// recv 완료 및 recv 다시 등록
	// 클라이언트 종료 처리는 함수가 오기 전 처리되어야 함.
	void OnRecvCompleted(const uint32 bytesTransferred);


	// send 시작
	void DoSend(const std::vector<char>& data);

	// 템플릿 버전 send
	template<typename T>
	void DoSend(const T& packet)
	{
		auto buffer{ Serializer::Serialize(packet) };
		DoSend(buffer);
	}

	// send 완료
	void OnSendCompleted(OverlappedEx* overlapped_ex);


	// IO 직후 세션과 연결이 끊겼을 경우 호출. reference counter 감소
	void Disconnect();


	// reference counting
	void IncreaseRef() { ++_referenceCount; };
	void ReleaseRef();


	// getter and setter
	void SetIOState(const IOState state) { _ioState = state; }
	IOState GetIOState() const { return _ioState; }

	int GetCurrentDataSize() const { return _currentDataSize; }

	std::array<char, BUFFER_SIZE>& GetRecvBuffer() { return _recvBuffer; }

	uint64 GetSessionID() const { return _sessionID; }

	// todo: thread unsafe
	std::shared_ptr<Room> GetRoom() const { return _room; }
	void SetRoom(const std::shared_ptr<Room>& room) { _room = room; }

	

private:
	// 패킷 재조립 및 패킷 처리
	void ReassemblePacket();

	// send 완료 후 overlapped 반환
	void ReleaseSend(OverlappedEx* over_ex);


private:

	// --
	// network info:
	// --
	uint64			_sessionID{ 0 };
	SOCKET			_clientSocket{ INVALID_SOCKET };
	std::atomic<IOState>	_ioState{ IOState::CONNECT };


	// --
	// content info:
	// --
	std::shared_ptr<Room>	_room{ nullptr };


	// --
	// RECV:
	// --
	// Recv 전용 Overlapped 변수
	OverlappedEx		_overlappedEx{};
	// 패킷 재조립 버퍼
	RecvBuffer			_recvBuffer{};
	// 남은 데이터 크기 
	uint32				_currentDataSize{ 0 };


	// --
	// SEND:
	// -- 
	// Send 전용 OverlappedEx
	// todo: thread unsafe
	std::array<OverlappedEx*, 10>	_sendOverlappedExArray{};
	std::queue<int>					_sendOverlappedExQueue{};


	// --
	// reference count:
	// --
	// 세션을 raw pointer로 사용하기 위한 카운터
	// IO 대기 중 shared_ptr로 만든 객체가 사라질 수 있어
	// 이를 방지하기 위해 IO 작업중인 횟수를 저장한다.
	std::atomic<int>	_referenceCount{ 0 };

};
