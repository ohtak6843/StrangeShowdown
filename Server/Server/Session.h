#pragma once

#include "OverlappedEx.h"

class Session : public std::enable_shared_from_this<Session>
{
public:

	Session() = delete;
	Session(const SOCKET client_socket, const uint64 id);


	// method

	// recv
	void DoRecv();
	// recv 완료 및 recv 다시 등록
	void OnRecvCompleted(const uint32 bytesTransferred);

	// send
	void DoSend(const std::vector<char>& data);
	// send 완료
	void OnSendCompleted();

	// reference counting
	void IncreaseRef() { ++_refCnt; };
	void ReleaseRef();

	// reference counting 및 recv 시작
	void Start();


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
	uint64			_sessionID{ 0 };
	SOCKET			_clientSocket{ INVALID_SOCKET };

	// RECV:
	// Overlapped 변수
	OverlappedEx	_overlappedEx{};
	// 패킷 재조립 버퍼
	std::array<char, BUFFER_SIZE> _recvBuffer{};
	// 남은 데이터 크기 
	uint32				_currentDataSize{ 0 };


	// reference count
	// 세션을 raw pointer로 사용하기 위한 카운터
	std::atomic<int>	_refCnt{ 0 };

};
