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
	bool Start();


	void DoSend(int client_id, void* packet);
	void Disconnect(const int client_id);


private:

	// GQCS를 통해 패킷을 recv하고 id에 맞추어서 dispatch하는 스레드.
	// I/O 완료 패킷을 다시 Room별 Queue에 넣는 Light Dispatcher 역할
	void GQCSThread();

	// GQCS Thread로부터 전달된 패킷을 방 별로 싱글 스레드로 처리하는 스레드.
	void WorkerThread();

	// listen socket accept
	void DoAccept();

	void DoRecv(std::shared_ptr<Session>& client_info) const;
	void DoSend(std::shared_ptr<Session>& client_info, void* packet);

	void SendPacketToWorker(int key, char* p);
	void ProcessPacket(int key, char* p);

private:

	HANDLE		_IOCPHandle{ INVALID_HANDLE_VALUE };
	int			_sessionCnt{ 0 };
	SOCKET		_listenSocket{ INVALID_SOCKET };
	SOCKET		_acceptSocket{ INVALID_SOCKET };
	OverlappedEx _acceptOverlappedEx{};

	std::vector<std::thread> _workers{};

	std::queue<std::pair<int, PacketType>> _packetQueue{};

	concurrency::concurrent_unordered_map<
		int, std::atomic<std::shared_ptr<Session>>> _sessionHash;

};

