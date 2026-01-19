#include "pch.h"
#include "session.h"
#include "OverlappedEx.h"
#include "IOCP.h"
#include "PacketHandler.h"

Session::Session(const SOCKET client_socket, const uint64 id)
	: _clientSocket{ client_socket }
	, _sessionID{ id }
{
	std::println("Session {} created.", _sessionID);
}

void Session::DoRecv()
{
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

	// todo: 예외 처리
	if (SOCKET_ERROR == ret)
	{
		int error{ WSAGetLastError() };
		if (WSA_IO_PENDING != error)
		{
			ReleaseRef(); 
		}
	}
}

void Session::OnRecvCompleted(const uint32 recieved_bytes)
{
	// todo: disconnect
	if (0 == recieved_bytes)
	{
		ReleaseRef();
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

void Session::DoSend()
{
	IncreaseRef();

	// todo:
	// 일단 임시로 new delete 사용, 임시로 login packet 전송
	// 나중에 shared ptr reference count 해결할 방도가 생각나면 
	auto overlapped_ex{ new OverlappedEx() };
	packet::SCLogin packet;

	// TODO: 직렬화 해주는 클래스 만들기
	// auto buffer{ packet.Serialize() };
	std::vector<char> buffer(packet.size);
	std::memcpy(buffer.data(), &packet, packet.size);

	overlapped_ex->PrepareSend(buffer);

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
			// 예외 처리
			delete overlapped_ex;
			ReleaseRef();
		}
	}
}

void Session::OnSendCompleted()
{
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
		uint32 packet_size{ static_cast<uint8>(_recvBuffer[0]) };

		// 패킷 처리 가능 여부 확인
		if (0u == packet_size && packet_size > _currentDataSize)
		{
			break;
		}

		// 패킷 처리
		PacketHandler::HandlePacket(shared_from_this(), _recvBuffer.data(), packet_size);
	
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

void Session::ReleaseRef()
{
	if (0 == --_refCnt) {
		GET_SINGLE(IOCP)->DeleteSession(_sessionID);
	}
}

void Session::Start()
{
	std::println("Session {} start.", _sessionID);

	// Start 세션 시작
	IncreaseRef();

	// Recv OverlappedEx 지정
	_overlappedEx.SetSession(shared_from_this());

	// recv 시작
	DoRecv();
	
	// Start 참조 해제
	ReleaseRef();
}