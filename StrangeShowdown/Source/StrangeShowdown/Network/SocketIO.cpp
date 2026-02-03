// Fill out your copyright notice in the Description page of Project Settings.


#include "Network/SocketIO.h"
#include "Sockets.h"
#include "StrangeShowdown.h"


// --
// RecvWorker
// --

RecvWorker::RecvWorker(FSocket* socket, TSharedPtr<SocketIO> socket_io) :
	Socket(socket),
	SocketIOPtr(socket_io)
{
	Thread = FRunnableThread::Create(this, TEXT("RecvWorkerThread"));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Created Recv Thread")));
}

RecvWorker::~RecvWorker()
{
	
}

bool RecvWorker::Init()
{
	return true;
}

uint32 RecvWorker::Run()
{
	while (Running)
	{
		DoRecv();
	}

	return 0;
}

void RecvWorker::Exit()
{
}

void RecvWorker::Destroy()
{
	Running = false;
}


void RecvWorker::DoRecv()
{
	TArray<uint8> buffer;
	uint32 header_size{ sizeof(packet::Header) };
	buffer.AddZeroed(header_size);

	// 헤더 recv
	if (false == RecvData(buffer.GetData(), header_size))
	{
		// todo: 오류 처리
		return;
	}

	// todo: 이를 Serlalizer로 옮기기
	uint32 remain_size{ buffer[0] - header_size };

	if (remain_size > 0)
	{
		// 추가 recv
		buffer.AddZeroed(remain_size);
		if (false == RecvData(buffer.GetData() + header_size, remain_size))
		{
			// todo: 오류 처리
			return;
		}
	}

	// 패킷 완성. SocketIO로 전달.
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Recv Success. Enqueue to SocketIO")));
	
	if (auto locked_ptr{ SocketIOPtr.Pin() })
	{
		locked_ptr->PushRecvPacket(buffer);
	}

	// todo: 임시로 cslogin 보내기

	// serlalize code
	// 
	//packet::CSLogin login_packet{};
	//TArray<uint8> data;
	//data.AddUninitialized(login_packet.size);
	//FMemory::Memcpy(data.GetData(), &login_packet, login_packet.size);
	//
	//if (auto locked_ptr{ SocketIOPtr.Pin() })
	//{
	//	locked_ptr->PushSendPacket(data);
	//}

}

bool RecvWorker::RecvData(uint8* data, const int32 size)
{
	int32 total{};

	// 원하는 크기만큼 수신될 때까지 반복
	while (total < size)
	{
		// recv
		int32 bytes_read{};
		auto ret{ Socket->Recv(data + total, size - total, bytes_read) };

		// 오류 처리
		if (false == ret || bytes_read <= 0)
		{
			return false;
		}

		// 읽은 만큼 누적
		total += bytes_read;
	}
	return true;
}


// --
// SendWorker
// --

SendWorker::SendWorker(FSocket* socket, TSharedPtr<SocketIO> socket_io) :
	Socket(socket),
	SocketIOPtr(socket_io)
{
	Thread = FRunnableThread::Create(this, TEXT("SendWorkerThread"));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Created Send Thread")));
}

SendWorker::~SendWorker()
{
}

bool SendWorker::Init()
{
	return true;
}

uint32 SendWorker::Run()
{
	while (Running)
	{
		DoSend();
	}

	return 0;
}

void SendWorker::Exit()
{

}

void SendWorker::Destroy()
{
	Running = false;
}

void SendWorker::DoSend()
{

	// SocketIO에서 보낼 패킷 꺼내오기
	TArray<uint8> buffer;
	if (auto locked_ptr{ SocketIOPtr.Pin() })
	{
		if (false == locked_ptr->PopSendPacket(buffer) || 0 == buffer.Num())
		{
			return;
		}
	}
	else
	{
		return;
	}

	int32 total{};
	int32 size{ static_cast<int32>(buffer[0]) };

	// 원하는 크기만큼 수신될 때까지 반복
	while (total < size)
	{
		// send
		int32 bytes_sent{};
		if (false == Socket->Send(buffer.GetData() + total, size - total, bytes_sent))
		{
			// todo: 오류 처리
			Running = false;
			return;
		}

		// 보낸 만큼 누적
		total += bytes_sent;
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Send Complete")));
}



// SocketIO

SocketIO::SocketIO(FSocket* InSocket) : Socket(InSocket)
{
}

SocketIO::~SocketIO()
{
}

void SocketIO::Init()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Init SocketIO")));
}

void SocketIO::Start()
{
	RecvThread = MakeShared<RecvWorker>(Socket, AsShared());
	SendThread = MakeShared<SendWorker>(Socket, AsShared());
}

void SocketIO::Disconnect()
{
	RecvThread->Destroy();
	SendThread->Destroy();
}

void SocketIO::PushRecvPacket(const TArray<uint8>& Packet)
{
	RecvPacketQueue.Enqueue(Packet);
}

bool SocketIO::PopRecvPacket(OUT TArray<uint8>& OutPacket)
{
	if (RecvPacketQueue.Dequeue(OUT OutPacket))
	{
		return true;
	}
	return false;
}

void SocketIO::PushSendPacket(const TArray<uint8>& Packet)
{
	SendPacketQueue.Enqueue(Packet);
}

bool SocketIO::PopSendPacket(OUT TArray<uint8>& OutPacket)
{

	if (SendPacketQueue.Dequeue(OUT OutPacket))
	{
		return true;
	}
	return false;
}