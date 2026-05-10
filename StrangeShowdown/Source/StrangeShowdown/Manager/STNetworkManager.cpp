// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/STNetworkManager.h"

#include "Sockets.h"
#include "Common/TcpSocketBuilder.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "SocketSubsystem.h"

#include "Network/SocketIO.h"
#include "Network/STSerializer.h"
#include "Network/STPacketHandler.h"


void USTNetworkManager::Start()
{
	TickHandle = FTSTicker::GetCoreTicker().AddTicker(
		FTickerDelegate::CreateUObject(this, &USTNetworkManager::Tick)
	);

	ConnectToGameServer();
}

void USTNetworkManager::BeginDestroy()
{
	Super::BeginDestroy();
	
	FTSTicker::GetCoreTicker().RemoveTicker(TickHandle);
	DisconnectFromGameServer();
}

void USTNetworkManager::ConnectToGameServer()
{
	Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(TEXT("Stream"), TEXT("Client Socket"));

	// IP
	FString	IpAddress{ TEXT("127.0.0.1") };
	FIPv4Address Ip;
	FIPv4Address::Parse(IpAddress, Ip);

	// Port 
	int Port{ 7777 };
	TSharedRef<FInternetAddr> InternetAddr{ ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr() };
	InternetAddr->SetIp(Ip.Value);
	InternetAddr->SetPort(Port);


	// Connect to server
	bool Connected{ Socket->Connect(*InternetAddr) };

	// Check Connection
	if (false == Connected)
	{
		// fail message
		UE_LOG(LogTemp, Log, TEXT("Failed to connect to server"));
		return;
	}

	// Handler Init
	PacketHandler = MakeShared<STPacketHandler>();

	// SocketIO Init
	SocketIOInstance = MakeShared<SocketIO>(Socket);
	SocketIOInstance->Init();
	SocketIOInstance->Start();

	// login packet Àü¼Û
	Common::CSLogin LoginPacket{};
	auto Packet{ STSerializer::Serialize(LoginPacket) };
	SendPacket(Packet);
}

void USTNetworkManager::DisconnectFromGameServer()
{
	if (Socket)
	{
		SocketIOInstance->Disconnect();
		Socket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Socket);
		while (false == SocketIOInstance->IsWorkerTerminated())
		{
			FPlatformProcess::Sleep(0.1f);
		}
		Socket = nullptr;
	}
}

void USTNetworkManager::HandleRecvPackets()
{

	while (true)
	{
		if (nullptr == Socket ||
			nullptr == SocketIOInstance ||
			true == IsLoadingLevel)
		{
			return;
		}

		TArray<uint8> Packet;
		if (false == SocketIOInstance->PopRecvPacket(Packet))
		{
			break;
		}

		PacketHandler->HandlePacket(Packet);
	}

}

bool USTNetworkManager::Tick(float DeltaTime)
{
	HandleRecvPackets();
	return true;
}

void USTNetworkManager::SendPacket(const TArray<uint8>& Packet)
{
	if (SocketIOInstance)
	{
		SocketIOInstance->PushSendPacket(Packet);
	}
}