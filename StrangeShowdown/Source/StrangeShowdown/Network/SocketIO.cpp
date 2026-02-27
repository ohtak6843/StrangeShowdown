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

	Terminated = true;
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
	uint32 header_size{ sizeof(Common::Header) };
	buffer.AddZeroed(header_size);

	// 헤더 recv
	if (false == RecvData(buffer.GetData(), header_size))
	{
		Running = false;
		return;
	}

	Common::Header& header{ *reinterpret_cast<Common::Header*>(buffer.GetData()) };

	uint32 remain_size{ header.size - header_size};

	if (remain_size > 0)
	{
		// 추가 recv
		buffer.AddZeroed(remain_size);
		if (false == RecvData(buffer.GetData() + header_size, remain_size))
		{
			Running = false;
			return;
		}
	}

	// 패킷 완성. SocketIO로 전달.
	if (auto locked_ptr{ SocketIOPtr.Pin() })
	{
		locked_ptr->PushRecvPacket(buffer);
	}
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

		// 오류 발생 시 스레드 종료
		// todo: socket close도 해줘야 할듯?
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
	Terminated = true;
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
		Running = false;
		return;
	}

	Common::Header& header{ *reinterpret_cast<Common::Header*>(buffer.GetData()) };

	int32 total{};
	int32 size{ static_cast<int32>(header.size) };

	// 원하는 크기만큼 수신될 때까지 반복
	while (total < size)
	{
		// send
		int32 bytes_sent{};
		auto ret{ Socket->Send(buffer.GetData() + total, size - total, bytes_sent) };

		// 오류 처리
		if (false == ret || bytes_sent <= 0)
		{
			Running = false;
			return;
		}

		// 보낸 만큼 누적
		total += bytes_sent;
	}

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

bool SocketIO::IsWorkerTerminated() const
{
	return RecvThread->IsTerminated() && SendThread->IsTerminated();
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