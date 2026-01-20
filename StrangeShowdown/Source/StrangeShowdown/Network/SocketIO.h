// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class SocketIO;

/**
 *  receive thread
 */
class STRANGESHOWDOWN_API RecvWorker : public FRunnable
{
public:
	RecvWorker(FSocket* socket, TSharedPtr<SocketIO> socket_io);
	virtual ~RecvWorker();

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Exit() override;

	void Destroy();

	// 패킷 수신
	void DoRecv();

	// 원하는 크기만큼 데이터 수신
	bool RecvData(uint8* data, const int32 size);

private:
	FSocket*			Socket;
	bool				Running{ true };
	FRunnableThread*	Thread{ nullptr };
	TWeakPtr<SocketIO>	SocketIOPtr;
};


/**
 *  send thread
 */
class STRANGESHOWDOWN_API SendWorker : public FRunnable
{
public:
	SendWorker(FSocket* socket, TSharedPtr<SocketIO> socket_io);
	virtual ~SendWorker();

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Exit() override;

	void Destroy();
	void DoSend();

private:
	FSocket*			Socket;
	bool				Running{ true };
	FRunnableThread*	Thread{ nullptr };
	TWeakPtr<SocketIO>	SocketIOPtr;
};


/**
 * Socket I/O 통신
 * TODO:
 * 나중에 NetworkGameInstance에서 하던 것을 SocketIO로 옮길 예정
 */
class STRANGESHOWDOWN_API SocketIO : public TSharedFromThis<SocketIO>
{
public:
	SocketIO() = delete;
	SocketIO(FSocket* InSocket);
	~SocketIO();

	void Init();
	void Start();
	void Disconnect();

	void PushRecvPacket(const TArray<uint8>& Packet);
	bool PopRecvPacket(OUT TArray<uint8>& OutPacket);

	void PushSendPacket(const TArray<uint8>& Packet);
	bool PopSendPacket(OUT TArray<uint8>& OutPacket);

private:
	FSocket* Socket{};
	TSharedPtr<RecvWorker> RecvThread{};
	TSharedPtr<SendWorker> SendThread{};

	// lf
	TQueue<TArray<uint8>> RecvPacketQueue{};
	TQueue<TArray<uint8>> SendPacketQueue{};
};