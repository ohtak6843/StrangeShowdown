#include "pch.h"
#include "session.h"
#include "OverlappedEx.h"
#include "IOCP.h"
#include "PacketHandler.h"
#include "RoomManager.h"
#include "Job.h"
#include "ObjectManager.h"


Session::Session()
{
}

void Session::Init(const SOCKET client_socket, const uint64 id)
{
	_clientSocket = client_socket;
	_sessionID = id;

	for (int i{}; i < 10; ++i)
	{
		_sendOverlappedExArray[i] = new OverlappedEx;
		_sendOverlappedExQueue.push(i);
	}
}

void Session::DoRecv()
{
	if (_ioState == IOState::DISCONNECT)
	{
		return;
	}

	// Recv IO 시작
	IncreaseRef();

	// overlappedEx 준비
	_overlappedEx.PrepareRecv();

	// 비동기 Recv
	DWORD recv_flag{ 0 };
	auto ret{ WSARecv(
		_clientSocket,
		&_overlappedEx.GetWsabuf(),
		1,
		0,
		&recv_flag,
		&_overlappedEx.GetOverlapped(),
		0)
	};

	if (SOCKET_ERROR == ret)
	{
		int error{ WSAGetLastError() };
		if (WSA_IO_PENDING != error)
		{
			printf("recv failed with error: %d\n", error);
			ReleaseRef(); 
		}
	}
}

void Session::OnRecvCompleted(const uint32 recieved_bytes)
{
	// 클라이언트 종료 처리는 OnRecv 호출 전 처리.
	if (0 == recieved_bytes)
	{
		Disconnect();
		return;
	}
	// 패킷 재조립 및 처리
	_currentDataSize += recieved_bytes;
	ReassemblePacket();
	
	// 다시 recv 시작
	DoRecv();

	// 이전 recv 작업 참조 해제
	ReleaseRef();
}

void Session::DoSend(const std::vector<char>& data)
{
	if (_ioState == IOState::DISCONNECT)
	{
		return;
	}

	IncreaseRef();

	
	// todo: thread unsafe
	while (true == _sendOverlappedExQueue.empty())
	{
		std::this_thread::yield();
	}
	auto index{ _sendOverlappedExQueue.front() };
	_sendOverlappedExQueue.pop();

	auto* overlapped_ex{ _sendOverlappedExArray[index] };
	overlapped_ex->PrepareSend(data, index);

	auto ret{ WSASend(
		_clientSocket,
		&overlapped_ex->GetWsabuf(),
		1,
		0,
		0,
		&overlapped_ex->GetOverlapped(),
		0)
	};

	// 예외 처리
	if (SOCKET_ERROR == ret)
	{
		int error{ WSAGetLastError() };
		if (WSA_IO_PENDING != error)
		{
			printf("send failed with error: %d\n", error);
			ReleaseSend(overlapped_ex);
			ReleaseRef();
		}
	}
}

void Session::OnSendCompleted(OverlappedEx* overlapped_ex)
{
	ReleaseSend(overlapped_ex);

	ReleaseRef();
}

void Session::ReassemblePacket()
{
	while (true)
	{
		// 패킷 크기를 읽을 수 없음.
		if (_currentDataSize < 1u)
		{
			break;
		}

		// 패킷 크기
		Common::Header& header{ *reinterpret_cast<Common::Header*>(_recvBuffer.data()) };
		uint32 packet_size{ header.size };

		// 패킷 처리 가능 여부 확인
		if (0 == packet_size && packet_size > _currentDataSize)
		{
			break;
		}

		// 패킷 오류 검사
		if (packet_size < sizeof(Common::Header) || packet_size > MAX_PACKET_SIZE)
		{
			std::println("Invalid packet size: {}. Disconnecting session {}.", packet_size, _sessionID);
			Disconnect();
			break;
		}

		// todo:
		// 현재 플레이어가 소속된 방이 있으면 방 작업 큐에 넣고
		// 아니면 바로 실행

		// 이 핸들패킷은 싱글스레드가 보장
		auto room{ GET_SINGLE(RoomManager)->GetRoom(_sessionID) };
		Job job{ [this]() { PacketHandler::HandlePacket(shared_from_this(), _recvBuffer); } };
		room->PushJob(job);

		// 이 핸들패킷은 멀티스레드.
		// 패킷핸들러를 바로 실행


		// 버퍼 당기기
		_currentDataSize -= packet_size;
		if (_currentDataSize > 0)
		{
			std::memmove(_recvBuffer.data(), _recvBuffer.data() + packet_size, _currentDataSize);
		}
		else
		{
			break;
		}
	}
}

void Session::ReleaseSend(OverlappedEx* over_ex)
{
	// todo: thread unsafe
	over_ex->Reset();
	_sendOverlappedExQueue.push(over_ex->GetSendIndex());
}

void Session::ReleaseRef()
{
	if (0 == --_referenceCount) {
		GET_SINGLE(IOCP)->DeleteSession(_sessionID);

		if (INVALID_SOCKET != _clientSocket)
		{
			closesocket(_clientSocket);
		}

		GET_SINGLE(RoomManager)->RemovePlayer(_sessionID);
		std::println("Session {} ended.", _sessionID);
	}
}

void Session::Start()
{
	std::println("Session {} start.", _sessionID);

	// Start 세션 시작
	IncreaseRef();

	_overlappedEx.SetSession(shared_from_this());

	// recv 시작
	DoRecv();
	
	// Start 참조 해제
	ReleaseRef();
}

void Session::Disconnect()
{
	_ioState = IOState::DISCONNECT;
	ReleaseRef();
}
