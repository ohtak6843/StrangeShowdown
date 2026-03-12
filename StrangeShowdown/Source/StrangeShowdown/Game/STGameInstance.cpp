// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/STGameInstance.h"
#include "GameFramework/GameUserSettings.h"

#include "Sockets.h"
#include "Common/TcpSocketBuilder.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "SocketSubsystem.h"
#include "Kismet/GameplayStatics.h"

#include "Network/SocketIO.h"
#include "Network/STSerializer.h"
#include "Network/STPacketHandler.h"

void USTGameInstance::Init()
{
	Super::Init();

	// GameUserSettings 설정 로드
	UGameUserSettings* UserSettings = GEngine->GetGameUserSettings();
	if (UserSettings)
	{
		UserSettings->LoadSettings(false);
	}

	// 해상도
	if (UserSettings)
	{
		FIntPoint Resolution = UserSettings->GetScreenResolution();
		UE_LOG(LogTemp, Log, TEXT("Loaded Screen Resolution: %dx%d"), Resolution.X, Resolution.Y);
		ScreenResolution = Resolution;
	}

	// 창모드
	if (UserSettings)
	{
		EWindowMode::Type FullscreenMode = UserSettings->GetFullscreenMode();
		UE_LOG(LogTemp, Log, TEXT("Loaded Fullscreen Mode: %d"), static_cast<int32>(FullscreenMode));
		WindowMode = FullscreenMode;
	}

#if NETWORK_ENABLED
	TickHandle = FTSTicker::GetCoreTicker().AddTicker(
		FTickerDelegate::CreateUObject(this, &USTGameInstance::GameInstanceTick)
	);
#endif // NETWORK_ENABLED
}

bool USTGameInstance::GameInstanceTick(float DeltaTime)
{
	HandleRecvPackets();
	return true;
}


void USTGameInstance::Shutdown()
{
	DisconnectFromGameServer();

	Super::Shutdown();
}

void USTGameInstance::AddRoom(USTRoomInfoObject* NewRoom)
{
	// 방 목록 추가
	RoomList.Add(NewRoom);

	// 방 목록이 업데이트되었음을 UI에게 알림
	OnRoomListUpdated.Broadcast();
}


void USTGameInstance::ConnectToGameServer()
{

#if NETWORK_ENABLED
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

	// login packet 전송
	Common::CSLogin LoginPacket{};
	auto Packet{ STSerializer::Serialize(LoginPacket) };
	SendPacket(Packet);

	UE_LOG(LogTemp, Log, TEXT("Success to connect to Server"));

#else

	UE_LOG(LogTemp, Log, TEXT("NETWORK_ENABLE is not defined."));
#endif
}



void USTGameInstance::DisconnectFromGameServer()
{

#if NETWORK_ENABLED
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
#endif
}

void USTGameInstance::HandleRecvPackets()
{
#if NETWORK_ENABLED
	if (nullptr == Socket || nullptr == SocketIOInstance)
		return;

	while (true)
	{
		TArray<uint8> Packet;
		if (false == SocketIOInstance->PopRecvPacket(Packet)) {
			break;
		}

		PacketHandler->HandlePacket(Packet);
	}
#endif
}

void USTGameInstance::HandleSpawn(const Common::SCSpawnObject& Packet)
{

	// transform
	FTransform transform{ FTransform::Identity };

	// todo: 수정 필요
	transform.SetLocation(FVector(Packet.pos.x, Packet.pos.y, Packet.pos.z));
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
	) };

	// playerid 넣기
	PlayerMap.Add(Packet.objectID, player);

}

void USTGameInstance::HandleMove(const Common::SCMovePlayer& Packet)
{
	if (ASTFieldPlayer * *player_ptr{ PlayerMap.Find(Packet.id) })
	{
		ASTFieldPlayer* player{ *player_ptr };

		FVector location{ Packet.pos.x, Packet.pos.y, Packet.pos.z };
		FRotator rotation{ Packet.dir.x, Packet.dir.y, Packet.dir.z };
		player->Move(location, rotation);
		player->PlayerStateFlag = Packet.state;
	}
}


void USTGameInstance::TempGetRoomList()
{
	Common::CSGetRoomList LoginPacket{};
	auto Packet{ STSerializer::Serialize(LoginPacket) };
	SendPacket(Packet);
	UE_LOG(LogTemp, Log, TEXT("[DEV CMD] ConnectServer called"));
}

void USTGameInstance::TempJoinRoom(uint32 RoomID)
{
	Common::CSJoinRoom JoinPacket{ RoomID };
	auto Packet{ STSerializer::Serialize(JoinPacket) };
	SendPacket(Packet);
	UE_LOG(LogTemp, Log, TEXT("[DEV CMD] JoinRoom called: RoomID=%d"), RoomID);
}

void USTGameInstance::TempCreateRoom()
{
	Common::CSCreateRoom CreatePacket{};
	auto Packet{ STSerializer::Serialize(CreatePacket) };
	SendPacket(Packet);
	UE_LOG(LogTemp, Log, TEXT("[DEV CMD] CreateRoom called"));
}

void USTGameInstance::TempChangeWorld()
{
	UGameplayStatics::OpenLevel(this, FName(TEXT("L_NetworkTestLevel")));
	UE_LOG(LogTemp, Log, TEXT("[DEV CMD] ChangeWorld called"));
}


void USTGameInstance::SendPacket(const TArray<uint8>& data)
{
	if (SocketIOInstance)
	{
		SocketIOInstance->PushSendPacket(data);
	}
}
