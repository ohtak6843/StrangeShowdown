#include "pch.h"
#include "session.h"
#include "OverlappedEx.h"
#include "IOCP.h"
#include "PacketHandler.h"
#include "RoomManager.h"
#include "Job.h"

Session::Session(const SOCKET client_socket, const uint64 id)
	: _clientSocket{ client_socket }
	, _sessionID{ id }
{
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

void Session::DoSend(const std::vector<char>& data)
{
	IncreaseRef();

	// todo:
	// 일단 임시로 new delete 사용, 임시로 login packet 전송
	// 나중에 shared ptr reference count 해결할 방도가 생각나면 
	auto overlapped_ex{ new OverlappedEx() };
	//packet::SCLogin packet;
	// TODO: 직렬화 해주는 클래스 만들기
	// auto buffer{ packet.Serialize() };
	//std::vector<char> buffer(packet.size);
	//std::memcpy(buffer.data(), &packet, packet.size);

	overlapped_ex->PrepareSend(data);

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
		if (0 == packet_size && packet_size > _currentDataSize)
		{
			break;
		}

		// todo:
		// 나중엔 room별 job 큐에 넣어야 함.
		// 임시로 패킷 처리를 여기서
		// PacketHandler::HandlePacket(shared_from_this(), _recvBuffer);

		// 현재 플레이어가 소속된 방이 있으면 방 작업 큐에 넣고
		// 아니면 바로 실행
		// 임시로 CS_Login은 바로 처리 -> 0번 방에 넣고 플레이어 생성 ( 나중에. 지금은 accept시 방에 넣기 )

		// 일단 지금 모두 방 매니저에서 방을 가져와 작업 큐에 넣기
		// 플레이어 세션id로 방을 가져올 수 있어야 함.
		// todo: [세션id, 방번호id] unordered_map이 있어야 할듯. -> 나중에 추가
		// Manager->GetRoom(SessionId)


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

void Session::ReleaseRef()
{
	if (0 == --_referenceCount) {
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