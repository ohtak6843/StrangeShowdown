// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/NetworkGameInstance.h"
#include "Sockets.h"
#include "Common/TcpSocketBuilder.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "SocketSubsystem.h"

#include "Network/SocketIO.h"
#include "Network/STSerializer.h"

void UNetworkGameInstance::ConnectToGameServer()
{
	// Socket Subsystem
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
	if (Connected)
	{	
		SocketIOInstance = MakeShared<SocketIO>(Socket);
		SocketIOInstance->Init();
		SocketIOInstance->Start();

		// login packet 전송
		packet::CSLogin LoginPacket{};
		auto Packet{ STSerializer::Serialize(LoginPacket) };
		SocketIOInstance->PushSendPacket(Packet);

		UE_LOG(LogTemp, Log, TEXT("Success to connect to Server"));
	}
	else
	{
		// fail message
		UE_LOG(LogTemp, Log, TEXT("Failed to connect to server"));
	}
}

void UNetworkGameInstance::DisconnectFromGameServer()
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

void UNetworkGameInstance::HandleRecvPackets()
{
	if (nullptr == Socket || nullptr == SocketIOInstance)
		return;

	while (true)
	{
		TArray<uint8> Packet;
		if (false == SocketIOInstance->PopRecvPacket(Packet)) {
			break;
		}
		
		// todo: packet 처리를 핸들러로
		auto Type{ static_cast<packet::Type>(Packet[1]) };
		switch (Type)
		{
		case packet::Type::SC_SPAWN_OBJECT:
		{
			auto SpawnPacket{ STSerializer::Deserialize<packet::SCSpawnObject>(Packet) };
			HandleSpawn(SpawnPacket);
			// ClientPacketHandler::HandlePacket(ThisPtr, Packet.GetData(), Packet.Num());
		}

		case packet::Type::SC_MOVE_OBJECT:
		{
			auto MovePacket{ STSerializer::Deserialize<packet::SCMoveObject>(Packet) };
			HandleMove(MovePacket);
			// ClientPacketHandler::HandlePacket(ThisPtr, Packet.GetData(), Packet.Num());
		}
		default:
			break;
		}
	}
}

void UNetworkGameInstance::HandleSpawn(const packet::SCSpawnObject& SpawnPacket)
{

	// transform
	FTransform transform{ FTransform::Identity };

	// todo: 수정 필요
	transform.SetLocation(FVector(SpawnPacket.pos.x, SpawnPacket.pos.y, SpawnPacket.pos.z));
	transform.SetRotation(FQuat::Identity);
	// 
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (nullptr == OtherPlayerClass)
	{
		UE_LOG(LogTemp, Log, TEXT("OtherPlayerClass is NOT assigned!"));
		return;
	}

	auto* player{ GetWorld()->SpawnActor<ASTFieldPlayer>(
		OtherPlayerClass,
		transform,
		SpawnParams
	)};
	
	// playerid 넣기
	PlayerMap.Add(SpawnPacket.objectID, player);

}

void UNetworkGameInstance::HandleMove(const packet::SCMoveObject& MovePacket)
{
	if (ASTFieldPlayer** player_ptr{ PlayerMap.Find(MovePacket.objectID) })
	{
		ASTFieldPlayer* player{ *player_ptr };

		FVector location{ MovePacket.pos.x, MovePacket.pos.y, MovePacket.pos.z };
		FRotator rotation{ MovePacket.dir.x, MovePacket.dir.y, MovePacket.dir.z };
		player->Move(location, rotation);
	}
}

void UNetworkGameInstance::SendPacket(const TArray<uint8>& data)
{
	if (SocketIOInstance)
	{
		SocketIOInstance->PushSendPacket(data);
	}
}
