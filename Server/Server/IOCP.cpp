#include "pch.h"
#include "IOCP.h"
#include "OverlappedEx.h"
#include "Session.h"


bool IOCP::Init()
{
	// 윈도우 초기화
	WSADATA wsadata;
	auto ret{ WSAStartup(MAKEWORD(2, 2), &wsadata) };
	if (0 != ret)
	{
		return false;
	}

	// listen socket 소켓 만들기
	_listenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == _listenSocket)
	{
		return false;
	}

	// 소켓 주소 설정
	SOCKADDR_IN server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT_NUMBER);
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;

	// bind
	ret = bind(_listenSocket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	if (0 != ret)
	{
		return false;
	}

	// listen
	ret = listen(_listenSocket, SOMAXCONN);
	if (0 != ret)
	{
		return false;
	}

	// IOCP 핸들 생성
	_IOCPHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);

	// 생성한 IOCP 핸들을 listen socket에 연결
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(_listenSocket), _IOCPHandle, 1, 0);

	std::println("IOCP Init Successed.");

	return true;
}

void IOCP::Start()
{
	// accept 
	DoAccept();

	auto thread_number {
		 static_cast<int>(std::thread::hardware_concurrency())
	};

	for (int i = 0; i < thread_number; ++i)
	{
		_workers.emplace_back([this]() { WorkerThread(); });
	}
}

void IOCP::DeleteSession(const uint64 session_id)
{
	// 추후 검색이 필요할 경우 LOCK 필요.
	_sessionHash.erase(session_id);
}


void IOCP::WorkerThread()
{

	// 순서

	// recv
	// -> 재조립하기
	// -> 각 방의 job 큐에 넣기
	// -> 다른 스레드가 업데이트중이 아니면 job을 모두 실행, 아니면 종료
	// 
	// send
	// -> MemoryManager에 사용한 Send OverlappedEx 반환하기



	while (true)
	{
		DWORD io_size;
		ULONG_PTR ul_session;
		WSAOVERLAPPED* over{ nullptr };

		// GQCS
		auto ret { GetQueuedCompletionStatus(
			_IOCPHandle,
			&io_size,
			&ul_session,
			&over,
			INFINITE)
		};

		// raw pointer 사용 시 주의
		OverlappedEx* curr_over_ex{ reinterpret_cast<OverlappedEx*>(over) };

		if (FALSE == ret)
		{
			// TODO:
			// Disconnect(key);
			continue;
		}

		// 완료된 작업의 OverlappedEx 정보를 읽는다.
		// 어떤 operation으로 완료되었는지 확인. 
		switch (curr_over_ex->GetOperation())
		{
		case IOOperation::ACCEPT:
		{
			OnAcceptCompleted();

			DoAccept();
		}
		break;

		case IOOperation::RECV:
		{
			Session* session{ reinterpret_cast<Session*>(ul_session) };
			session->OnRecvCompleted(io_size);
		}
		break;

		case IOOperation::SEND:
		{
			Session* session{ reinterpret_cast<Session*>(ul_session) };
			session->OnSendCompleted();

			// todo: 이 delete를 OnSendCompleted에서 메모리 풀 반납.
			delete curr_over_ex;
			break;
		}
		break;

		}
	}

	
	//// 여기를 여러 스레드에서 동시에 방을 업데이트 해주도록 변경해야 한다.
	//while (true) {
	//	// 패킷 반영
	//	while (not _packetQueue.empty())
	//	{
	//		auto& packet{ _packetQueue.front() };
	//		ProcessPacket(packet.first, packet.second.data());
	//		_packetQueue.pop();
	//	}
	//	// 업데이트 
	//	// GET_SINGLE(Game)->Update();
	//}

}

void IOCP::DoAccept()
{
	// accept를 위한 새로운 소켓 생성
	_acceptSocket = WSASocket(
		AF_INET,
		SOCK_STREAM,
		0,
		NULL,
		0,
		WSA_FLAG_OVERLAPPED
	);
	int flag{ 1 };
	setsockopt(_acceptSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(flag));

	// accept를 위한 overlappedEx 설정
	_acceptOverlappedEx.PrepareAccept();

	// accept
	DWORD bytes_received{};
	AcceptEx(
		_listenSocket,
		_acceptSocket,
		_acceptBuffer.data(),
		0,
		ADDR_SIZE,
		ADDR_SIZE,
		&bytes_received,
		&_acceptOverlappedEx.GetOverlapped()
	);

}

void IOCP::OnAcceptCompleted()
{
	// 세션 맵에 저장 위한 ID
	// accept는 한 스레드에서만 실행되므로 문제 없음.
	auto id{ _sessionCnt++ };

	// 새로운 세션 생성
	std::shared_ptr<Session>
		new_client{ std::make_shared<Session>(_acceptSocket, id) };

	// IOCP 객체에 받아들인 클라이언트의 소켓을 연결.
	// 이때, key는 세션 포인터로 전달
	auto ret{ CreateIoCompletionPort(
		reinterpret_cast<HANDLE>(_acceptSocket),
		_IOCPHandle,
		reinterpret_cast<ULONG_PTR>(new_client.get()),
		0)
	};

	// TODO:
	// 예외 처리		
	if (ret == NULL) {
		//Disconnect(key);
		//break;
	}

	// sessionHash에 클라이언트 정보 저장
	_sessionHash[id] = new_client;

	// 세션 시작
	new_client->Start();
}

// TODO:
// Session으로 이관 및 send 전용 OverlappedEx 풀을 통해 Send.
//void IOCP::DoSend(std::shared_ptr<Session>& client_info, void* packet)
//{
//	if (nullptr == client_info) { return; }
//	OverlappedEx* send_over_ex{ new OverlappedEx{ reinterpret_cast<unsigned char*>(packet) } };
//	WSASend(client_info->ClientSocket, &send_over_ex->wsabuf, 1, 0, 0, &send_over_ex->over, 0);
//}
//
//void IOCP::DoSend(int client_id, void* packet)
//{
//	auto client{ _sessionHash.at(client_id).load() };
//	DoSend(client, packet);
//}
//
//void IOCP::SendPacketToWorker(int key, char* p)
//{
//	PacketType packet{};
//	std::copy(p, p + BUFFER_SIZE, packet.data());
//	_packetQueue.push(std::make_pair(key, packet));
//	// ProcessPacket(key, p);
//}
//
//void IOCP::ProcessPacket(int key, char* p)
//{
//
//	// IOCP에서 처리
//	packet::Header* header{ reinterpret_cast<packet::Header*>(p) };
//	auto client{ _sessionHash.at(key).load() };
//	if (nullptr == client) {
//		return;
//	}
//
//	switch (header->type)
//	{
//	case packet::Type::CS_LOGIN:
//	{
//		packet::SCLogin sc_login{};
//		DoSend(key, &sc_login);
//	}
//	break;
//
//	//case packet::Type::CS_MATCHMAKING:
//	//{
//
//	//	// TODO: 나중에는 역할군 선택 기능도 지원해야 함.
//	//	auto* packet{ reinterpret_cast<packet::CSMatchmaking*>(p) };
//
//	//	// 역할군 저장
//	//	auto client{ _sessionHash.at(key).load() };
//	//	if (nullptr == client) {
//	//		break;
//	//	}
//
//	//}
//	break;
//
//	}
//
//
//}

// TODO:
// sessionhash 개선 이후 손보기
//void IOCP::Disconnect(const int client_id)
//{
//	auto client{ _sessionHash.at(client_id).load() };
//	if (nullptr == client) {
//		return;
//	}
//	client->IOState = IOState::DISCONNECT;
//	closesocket(client->ClientSocket);
//}



IOCP::~IOCP()
{
	for (auto& thread : _workers)
	{
		if (thread.joinable())
		{
			thread.join();
		}
	}
	closesocket(_listenSocket);
	WSACleanup();
}
