// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/NetworkGameInstance.h"
#include "Sockets.h"
#include "Common/TcpSocketBuilder.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "SocketSubsystem.h"

#include "Network/SocketIO.h"

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


		// todo: 임시로 CS_LOGIN 패킷 전송
		// todo: 나중에 함수화
		TArray<uint8> Packet;
		packet::CSLogin login_packet{};
		Packet.AddUninitialized(login_packet.size);
		FMemory::Memcpy(Packet.GetData(), &login_packet, login_packet.size);
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

		//ISocketSubsystem* SocketSubsystem{ ISocketSubsystem::Get() };
		//SocketSubsystem->DestroySocket(Socket);
		//Socket = nullptr;
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
		
		// todo: 이 스위치를 변경
		auto type{ static_cast<packet::Type>(Packet[1]) };
		switch (type)
		{
		case packet::Type::SC_SPAWN_OBJECT:
		{
			auto* LoginPacket{ reinterpret_cast<packet::SCSpawnObject*>(Packet.GetData()) };
			HandleSpawn(LoginPacket);
			// ClientPacketHandler::HandlePacket(ThisPtr, Packet.GetData(), Packet.Num());
		}

		case packet::Type::SC_MOVE_OBJECT:
		{
			auto* LoginPacket{ reinterpret_cast<packet::SCMoveObject*>(Packet.GetData()) };
			HandleMove(LoginPacket);
			// ClientPacketHandler::HandlePacket(ThisPtr, Packet.GetData(), Packet.Num());
		}
		default:
			break;
		}
	}
}

void UNetworkGameInstance::HandleSpawn(packet::SCSpawnObject* spawn_packet)
{

	// transform
	FTransform transform{ FTransform::Identity };

	// todo: 수정 필요
	transform.SetLocation(FVector(spawn_packet->pos.x, spawn_packet->pos.y, spawn_packet->pos.z));
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
	PlayerMap.Add(spawn_packet->objectID, player);

}

void UNetworkGameInstance::HandleMove(packet::SCMoveObject* move_packet)
{
	if (ASTFieldPlayer** player_ptr{ PlayerMap.Find(move_packet->objectID) })
	{
		ASTFieldPlayer* player{ *player_ptr };

		FVector location{ move_packet->pos.x, move_packet->pos.y, move_packet->pos.z };
		FRotator rotation{ move_packet->dir.x, move_packet->dir.y, move_packet->dir.z };
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
