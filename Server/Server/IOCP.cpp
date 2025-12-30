#include "pch.h"
#include "IOCP.h"
#include "OverlappedEx.h"
#include "Session.h"


bool IOCP::Init()
{
	// 윈도우 초기화
	WSADATA wsadata;
	auto ret = WSAStartup(MAKEWORD(2, 2), &wsadata);
	if (0 != ret) {
		// handle error
		return false;
	}

	// listen socket 소켓 만들기
	_listenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == _listenSocket) {
		// handle error
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
	if (0 != ret) {
		// handle error
		return false;
	}

	// listen
	ret = listen(_listenSocket, SOMAXCONN);
	if (0 != ret) {
		// handle error
		return false;
	}

	// IOCP 핸들 생성
	_IOCPHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);

	// 생성한 IOCP 핸들을 listen socket에 연결
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(_listenSocket), _IOCPHandle, 99999, 0);

	std::println("IOCP Init Successed.");

	return true;
}

bool IOCP::Start()
{
	DoAccept();

	auto thread_number{
		// static_cast<int>(std::thread::hardware_concurrency())
		4
	};

	for (int i = 0; i < thread_number; ++i) {
		_workers.emplace_back([this]() { GQCSThread(); });
	}


	//for (int i = 0; i < thread_number; ++i) {
	// _workers.emplace_back([this]() { WorkerThread(); });
	//}


	return true;
}

void IOCP::GQCSThread()
{
	while (true) {

		DWORD io_size;
		ULONG_PTR ul_key;
		WSAOVERLAPPED* over{ nullptr };

		// GQCS
		auto ret{ GetQueuedCompletionStatus(
			_IOCPHandle,
			&io_size,
			&ul_key,
			&over,
			INFINITE) };

		OverlappedEx* curr_over_ex{ reinterpret_cast<OverlappedEx*>(over) };

		int key{ static_cast<int>(ul_key) };

		if (FALSE == ret) {
			Disconnect(key);
			continue;
		}

		// 완료된 작업의 OverlappedEx 정보를 읽는다.
		// 어떤 operation으로 완료되었는지 확인. 
		switch (curr_over_ex->operation) {
		case IOOperation::ACCEPT:
		{
			int client_id{ _sessionCnt++ };

			std::shared_ptr<Session>
				new_client{ std::make_shared<Session>(_acceptSocket) };

			// IOCP 객체에 받아들인 클라이언트의 소켓을 연결
			auto ret = CreateIoCompletionPort(
				reinterpret_cast<HANDLE>(_acceptSocket),
				_IOCPHandle,
				client_id,
				0);

			// ret 처리			
			if (ret == NULL) {
				Disconnect(key);
				break;
			}

			// sessionHash에 클라이언트 정보 저장
			_sessionHash.insert(std::make_pair(client_id, new_client));

			DoRecv(new_client);


			DoAccept();
		}
		break;

		case IOOperation::RECV:
		{
			auto client{ _sessionHash.at(key).load() };
			if (nullptr == client) {
				break;
			}

			int remain_data{ static_cast<int>(io_size) + client->CurrentDataSize };
			auto* p{ curr_over_ex->dataBuffer.data() };


			int packet_size{ p[0] };

			// 패킷 처리가 가능하면
			while (packet_size <= remain_data) {

				// worker에게 패킷을 전달
				SendPacketToWorker(key, p);

				// 다음 패킷 위치로 땡기기
				p = p + packet_size;
				remain_data = remain_data - packet_size;
			}


			// dataBuffer 정상화
			client->CurrentDataSize = remain_data;
			if (remain_data > 0) {
				memcpy(curr_over_ex->dataBuffer.data(), p, remain_data);
			}
			DoRecv(client);
		}
		break;

		case IOOperation::SEND:
		{
			// 보낸 OVER_EXP 지우기.
			delete curr_over_ex;
			break;
		}
		break;

		}
	}
}

void IOCP::WorkerThread()
{
	// 여기를 여러 스레드에서 동시에 방을 업데이트 해주도록 변경해야 한다.
	while (true) {
		// 패킷 반영
		while (not _packetQueue.empty())
		{
			auto& packet{ _packetQueue.front() };
			ProcessPacket(packet.first, packet.second.data());
			_packetQueue.pop();
		}
		// 업데이트 
		// GET_SINGLE(Game)->Update();
	}
}

// do recv, 이건 세션에 넣기
void IOCP::DoRecv(std::shared_ptr<Session>& session) const
{

	DWORD recv_flag = 0;
	OverlappedEx& over_ex = session->OverEx;
	ZeroMemory(&over_ex, sizeof(over_ex));

	// databuffer에는 안에 남은 정보가 저장되어 있다.
	// 이미 받은 정보는 제외 하고 안 받은 부분 부터 recv 해야 한다. 
	over_ex.wsabuf.len = BUFFER_SIZE - session->CurrentDataSize;
	over_ex.wsabuf.buf = over_ex.dataBuffer.data() + session->CurrentDataSize;
	over_ex.operation = IOOperation::RECV;

	// 비동기 Recv
	WSARecv(
		session->ClientSocket,
		&over_ex.wsabuf,
		1,
		0,
		&recv_flag,
		&over_ex.over,
		0);
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
		WSA_FLAG_OVERLAPPED);
	int flag{ 1 };
	setsockopt(_acceptSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(flag));

	// accept를 위한 overlappedEx 설정
	ZeroMemory(&_acceptOverlappedEx, sizeof(OverlappedEx));
	_acceptOverlappedEx.operation = IOOperation::ACCEPT;
	_acceptOverlappedEx.clientSocket = _acceptSocket;


	// accept
	int addr_size{ sizeof(SOCKADDR_IN) };
	DWORD bytes_received{};
	AcceptEx(
		_listenSocket,
		_acceptSocket,
		_acceptOverlappedEx.dataBuffer.data(),
		0,
		addr_size + 16,
		addr_size + 16,
		&bytes_received,
		&_acceptOverlappedEx.over
	);

}

void IOCP::DoSend(std::shared_ptr<Session>& client_info, void* packet)
{
	if (nullptr == client_info) { return; }
	OverlappedEx* send_over_ex{ new OverlappedEx{ reinterpret_cast<unsigned char*>(packet) } };
	WSASend(client_info->ClientSocket, &send_over_ex->wsabuf, 1, 0, 0, &send_over_ex->over, 0);
}

void IOCP::DoSend(int client_id, void* packet)
{
	auto client{ _sessionHash.at(client_id).load() };
	DoSend(client, packet);
}

void IOCP::SendPacketToWorker(int key, char* p)
{
	PacketType packet{};
	std::copy(p, p + BUFFER_SIZE, packet.data());
	_packetQueue.push(std::make_pair(key, packet));
	// ProcessPacket(key, p);
}

void IOCP::ProcessPacket(int key, char* p)
{

	// IOCP에서 처리
	packet::Header* header{ reinterpret_cast<packet::Header*>(p) };
	auto client{ _sessionHash.at(key).load() };
	if (nullptr == client) {
		return;
	}

	switch (header->type)
	{
	case packet::Type::CS_LOGIN:
	{
		packet::SCLogin sc_login{};
		DoSend(key, &sc_login);
	}
	break;

	//case packet::Type::CS_MATCHMAKING:
	//{

	//	// TODO: 나중에는 역할군 선택 기능도 지원해야 함.
	//	auto* packet{ reinterpret_cast<packet::CSMatchmaking*>(p) };

	//	// 역할군 저장
	//	auto client{ _sessionHash.at(key).load() };
	//	if (nullptr == client) {
	//		break;
	//	}

	//}
	break;

	}


}

void IOCP::Disconnect(const int client_id)
{
	auto client{ _sessionHash.at(client_id).load() };
	if (nullptr == client) {
		return;
	}
	client->IOState = IOState::DISCONNECT;
	closesocket(client->ClientSocket);
}



IOCP::~IOCP()
{
	for (auto& thread : _workers) {
		if (thread.joinable()) {
			thread.join();
		}
	}
	closesocket(_listenSocket);
	WSACleanup();
}
