#pragma once

#include "session.h"
#include "OverlappedEx.h"


class IOCP
{
	DECLARE_SINGLE(IOCP)

public:
	~IOCP();

public:
	bool Init();
	void Start();

	// void Disconnect(Session* client_id);

	void DeleteSession(const uint64 session_id);

private:

	// GQCS를 통해 패킷을 받고 처리.
	void WorkerThread();

	// accept
	void DoAccept();
	void OnAcceptCompleted();

private:

	// iocp handle
	HANDLE		_IOCPHandle{ INVALID_HANDLE_VALUE };

	// listen
	SOCKET		_listenSocket{ INVALID_SOCKET };

	// accept
	SOCKET		_acceptSocket{ INVALID_SOCKET };
	OverlappedEx _acceptOverlappedEx{};

	// todo: over 내부 버퍼를 활용해보자
	// accpet 전용. 실제로 사용하지는 않음.
	std::array<char, BUFFER_SIZE> _acceptBuffer{};

	// threads
	std::vector<std::thread> _workers{};

	// 특수 상황 조회 시, 생명주기 관리 ( 안전성 )
	uint64	_sessionCnt{ 0 };

	// todo: 이걸 private화
public:
	std::unordered_map<uint64, std::shared_ptr<Session>> _sessionHash{};

};

