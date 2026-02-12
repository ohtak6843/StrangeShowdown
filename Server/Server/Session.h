#pragma once

#include "OverlappedEx.h"
#include "Serializer.h"

class Session : public std::enable_shared_from_this<Session>
{
public:

	Session() = delete;
	Session(const SOCKET client_socket, const uint64 id);

	~Session();

	// method

	// recv
	void DoRecv();
	// recv 완료 및 recv 다시 등록
	// 클라이언트 종료 처리는 함수가 오기 전 처리되어야 함.
	void OnRecvCompleted(const uint32 bytesTransferred);

	// send
	void DoSend(const std::vector<char>& data);

	template<typename T>
	void DoSend(const T& packet)
	{
		auto buffer{ Serializer::Serialize(packet) };
		DoSend(buffer);
	}


	// send 완료
	void OnSendCompleted();

	// reference counting
	void IncreaseRef() { ++_referenceCount; };
	void ReleaseRef();

	// reference counting 및 recv 시작
	void Start();

	// IO 직후 세션과 연결이 끊겼을 경우 호출. reference counter 감소
	void Disconnect();

	// getter and setter

	int GetCurrentDataSize() const { return _currentDataSize; }
	std::array<char, BUFFER_SIZE>& GetRecvBuffer() { return _recvBuffer; }

	void SetSessionID(const uint64 id) { _sessionID = id; }
	uint64 GetSessionID() const { return _sessionID; }

	

private:
	// 패킷 재조립 및 패킷 처리
	void ReassemblePacket();


private:

	// network info
	// pointer가 아닌 별도 id
	uint64			_sessionID{ 0 };
	SOCKET			_clientSocket{ INVALID_SOCKET };
	std::atomic<IOState>	_ioState{ IOState::CONNECT };

	// RECV:
	// Overlapped 변수
	OverlappedEx	_overlappedEx{};
	// 패킷 재조립 버퍼
	RecvBuffer		_recvBuffer{};
	
	// 남은 데이터 크기 
	uint32				_currentDataSize{ 0 };

	// reference count
	// 세션을 raw pointer로 사용하기 위한 카운터
	// IO 대기 중 shared_ptr로 만든 객체가 사라질 수 있어
	// 이를 방지하기 위해 IO 작업중인 횟수를 저장한다.
	std::atomic<int>	_referenceCount{ 0 };

	// additional info
};
