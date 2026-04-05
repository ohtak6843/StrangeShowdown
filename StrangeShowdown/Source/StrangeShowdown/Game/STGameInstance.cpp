// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/STGameInstance.h"
#include "GameFramework/GameUserSettings.h"

#include "Sockets.h"
#include "Common/TcpSocketBuilder.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "SocketSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Interface/STControllerHUDInterface.h"
#include "Widget/STChatManagerWidget.h"

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
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &USTGameInstance::OnLevelLoaded);
	
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
#ifdef NETWORK_ENABLED
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);
	FTSTicker::GetCoreTicker().RemoveTicker(TickHandle);


	DisconnectFromGameServer();

#endif // NETWORK_ENABLED
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

	while (true)
	{
		if (nullptr == Socket ||
			nullptr == SocketIOInstance ||
			true == IsLoadingLevel)
		{
			return;
		}

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
	if (ASTFieldPlayer **PlayerPtr{ PlayerMap.Find(Packet.id) })
	{
		ASTFieldPlayer* Player{ *PlayerPtr };
		
		if (false == IsValid(Player))
		{
			UE_LOG(LogTemp, Warning, TEXT("Player with ID %d is not valid"), Packet.id);
			return;
		}

		FVector location{ Packet.pos.x, Packet.pos.y, Packet.pos.z };
		FRotator rotation{ Packet.dir.x, Packet.dir.y, Packet.dir.z };
		Player->Move(location, rotation);
		Player->PlayerStateFlag = Packet.state;
	}
}

void USTGameInstance::HandleGiveRoomList(const Common::SCGiveRoomList& Packet, const uint8* PayloadPtr, const uint16 PayloadSize)
{
	RoomList.Empty();

	const auto* Rooms{ reinterpret_cast<const Common::RoomInfo*>(PayloadPtr) };

	for (uint16 i{}; i < Packet.roomCount; ++i)
	{
		auto& Info{ Rooms[i] };
		USTRoomInfoObject* TestRoom{ NewObject<USTRoomInfoObject>() };
		TestRoom->RoomInfo.RoomName = FString{ UTF8_TO_TCHAR(Info.name) };
		TestRoom->RoomInfo.CurrentPlayers = static_cast<int32>(Rooms[i].currentPlayerCount);
		TestRoom->RoomInfo.MaxPlayers = 6;
		TestRoom->RoomInfo.bHasPassword = Info.hasPassword;
		TestRoom->RoomInfo.Password = TEXT("");
		TestRoom->RoomInfo.ID = static_cast<int64>(Info.roomId);
		RoomList.Add(TestRoom);

		UE_LOG(LogTemp, Log, TEXT("Room Get Success. RoomID: %d, CurrentPlayers: %d, HasPassword: %d, RoomName: %s"),
			Info.roomId,
			Info.currentPlayerCount,
			Info.hasPassword,
			*FString{ UTF8_TO_TCHAR(Info.name) }
		);
	}

	OnRoomListUpdated.Broadcast();
}

void USTGameInstance::HandleChat(const Common::SCChat& Packet, const uint8* PayloadPtr, const uint16 PayloadSize)
{
	FString Message{ UTF8_TO_TCHAR(reinterpret_cast<const char*>(PayloadPtr)) };
	UE_LOG(LogTemp, Log, TEXT("Chat Message Received By %d: %s"), Packet.id, *Message);

	APlayerController* PlayerController{ UGameplayStatics::GetPlayerController(GetWorld(), 0) };
	ISTControllerHUDInterface* HUDInterface{ Cast<ISTControllerHUDInterface>(PlayerController) };
	if (nullptr == HUDInterface)
	{
		return;
	}

	HUDInterface->GetChatManagerWidget()->AddChatMessage("Player", Message);
}

void USTGameInstance::HandleJoinRoom(const Common::SCJoinRoom& Packet)
{
	if (true == Packet.success)
	{
		UE_LOG(LogTemp, Log, TEXT("Successfully joined the room"));
		ChangeWorld(INVTEXT("L_Lobby"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Failed to join the room"));
	}
}

void USTGameInstance::GetRoomList()
{
	Common::CSGetRoomList GetRoomListPacket{};
	auto Packet{ STSerializer::Serialize(GetRoomListPacket) };
	SendPacket(Packet);
	UE_LOG(LogTemp, Log, TEXT("GetRoomList called"));
}

void USTGameInstance::JoinRoom(const int64 RoomID, const FText& Password)
{
	Common::CSJoinRoom JoinPacket{
		static_cast<uint32>(RoomID),
		""
	};
	UE_LOG(LogTemp, Log, TEXT("JoinRoom Password=%s"), *FString{ Password.ToString() });
	strcpy_s(JoinPacket.password, TCHAR_TO_UTF8(*Password.ToString()));

	auto Packet{ STSerializer::Serialize(JoinPacket) };
	SendPacket(Packet);
	UE_LOG(LogTemp, Log, TEXT("JoinRoom called: RoomID=%d"), RoomID);
}

void USTGameInstance::CreateRoom(const FText& Name, const FText& Password)
{
	Common::CSCreateRoom CreatePacket{
		"",
		false,
		""
	};
	UE_LOG(LogTemp, Log, TEXT("CreateRoom Password=%s"), *FString{ Password.ToString() });
	strcpy_s(CreatePacket.name, TCHAR_TO_UTF8(*Name.ToString()));
	strcpy_s(CreatePacket.password, TCHAR_TO_UTF8(*Password.ToString()));
	if (false == Password.IsEmpty())
	{
		CreatePacket.hasPassword = true;
	}

	auto Packet{ STSerializer::Serialize(CreatePacket) };
	SendPacket(Packet);
	UE_LOG(LogTemp, Log, TEXT("CreateRoom called"));
}

void USTGameInstance::ChangeWorld(const FText& Level)
{
	IsLoadingLevel = true;
	//PlayerMap.Empty();

	FName LevelName(*Level.ToString());
	UGameplayStatics::OpenLevel(this, LevelName);
	UE_LOG(LogTemp, Log, TEXT("ChangeWorld called"));
}

void USTGameInstance::Chat(const FText& Message)
{
	FString MessageStr{ Message.ToString() };
	FTCHARToUTF8 Utf8String(*MessageStr);

	TArray<uint8> AdditionalData;
	AdditionalData.Append(reinterpret_cast<const uint8*>(Utf8String.Get()), Utf8String.Length());

	Common::CSChat ChatPacket{ static_cast<uint16>(AdditionalData.Num()) };
	auto Buffer{ STSerializer::Serialize(ChatPacket, AdditionalData) };
	
	SendPacket(Buffer);
	UE_LOG(LogTemp, Log, TEXT("Chat called: Message=%s"), *Message.ToString());
}

void USTGameInstance::DevGetRoomList()
{
	GetRoomList();
}

void USTGameInstance::DevJoinRoom(const uint32 RoomID)
{
	JoinRoom(RoomID);
}

void USTGameInstance::DevCreateRoom(const FString& RoomName, const FString& Password)
{
	FText TextRoomName = FText::FromString(RoomName);
	FText TextPassword = FText::FromString(Password);

	CreateRoom(TextRoomName, TextPassword);
}

void USTGameInstance::DevChangeWorld(const FString& Level)
{
	ChangeWorld(FText::FromString(Level));
}

void USTGameInstance::DevChat(const FString& Message)
{
	FText TextMessage = FText::FromString(Message);
	Chat(TextMessage);
}


void USTGameInstance::SendPacket(const TArray<uint8>& data)
{
	if (SocketIOInstance)
	{
		SocketIOInstance->PushSendPacket(data);
	}
}

void USTGameInstance::OnLevelLoaded(UWorld* LoadedWorld)
{
	if (LoadedWorld)
	{
		UE_LOG(LogTemp, Log, TEXT("Level loaded: %s"), *LoadedWorld->GetName());
		IsLoadingLevel = false;
	}
}
