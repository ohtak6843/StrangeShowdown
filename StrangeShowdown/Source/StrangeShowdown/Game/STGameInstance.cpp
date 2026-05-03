// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/STGameInstance.h"
#include "GameFramework/GameUserSettings.h"

#include "Sockets.h"
#include "Common/TcpSocketBuilder.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "SocketSubsystem.h"

#include "Manager/STNetworkManager.h"
#include "Manager/STDataManager.h"

#include "Network/SocketIO.h"
#include "Network/STSerializer.h"
#include "Network/STPacketHandler.h"

#include "Kismet/GameplayStatics.h"
#include "Interface/STControllerHUDInterface.h"
#include "UI/ChatManager/STChatManagerWidget.h"


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

	if (nullptr == NetworkManager)
	{
		NetworkManager = NewObject<USTNetworkManager>(this);
	}

	if (nullptr == DataManager)
	{
		DataManager = NewObject<USTDataManager>(this);
	}

	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &USTGameInstance::OnLevelLoaded);
	
#endif // NETWORK_ENABLED
}

void USTGameInstance::OnStart()
{
	Super::OnStart();

#if NETWORK_ENABLED

	NetworkManager->Start();

#endif // NETWORK_ENABLED
}

void USTGameInstance::Shutdown()
{
	Super::Shutdown();

#ifdef NETWORK_ENABLED
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);
	

#endif // NETWORK_ENABLED
}

void USTGameInstance::AddRoom(USTRoomInfoObject* NewRoom)
{
	// 방 목록 추가
	RoomList.Add(NewRoom);

	// 방 목록이 업데이트되었음을 UI에게 알림
	OnRoomListUpdated.Broadcast();
}


void USTGameInstance::HandleSpawn(const Common::SCSpawnObject& Packet)
{
	// player bp class
	DataManager->HandleSpawn(Packet);

}

void USTGameInstance::HandleMove(const Common::SCMovePlayer& Packet)
{
	DataManager->HandleMove(Packet);
}

void USTGameInstance::HandleCreateRoom(const Common::SCCreateRoom& Packet)
{
	UE_LOG(LogTemp, Log, TEXT("Room Create: %s"), Packet.success ? TEXT("true") : TEXT("false"));
	if (true == Packet.success)
	{
		DataManager->HandleCreateRoom(Packet);
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
	FUTF8ToTCHAR ConvertedStr(reinterpret_cast<const char*>(PayloadPtr), PayloadSize);
	FString Message(ConvertedStr.Length(), ConvertedStr.Get());

	UE_LOG(LogTemp, Log, TEXT("Chat Message Received By %d length %d : %s"), Packet.id, PayloadSize, *Message);

	auto* PlayerController{ UGameplayStatics::GetPlayerController(GetWorld(), 0) };
	auto* HUDInterface{ Cast<ISTControllerHUDInterface>(PlayerController) };
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
		DataManager->HandleJoinRoom(Packet);
		ChangeWorld(INVTEXT("L_Lobby"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Failed to join the room"));
	}
}

void USTGameInstance::HandleReady(const Common::SCReady& Packet)
{
	DataManager->HandleReady(Packet);
	UE_LOG(LogTemp, Log, TEXT("Player %d is %s"), Packet.id, Packet.ready ? TEXT("ready") : TEXT("not ready"));
}

void USTGameInstance::HandleStartGame(const Common::SCStartGame& Packet)
{
	UE_LOG(LogTemp, Log, TEXT("Game Start: %s"), Packet.start ? TEXT("true") : TEXT("false"));
	if (true == Packet.start)
	{
		DataManager->HandleStartGame(Packet);
		ChangeWorld(INVTEXT("L_InGame"));
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

void USTGameInstance::Ready(bool Value)
{
	Common::CSReady ReadyPacket{ Value };
	auto Packet{ STSerializer::Serialize(ReadyPacket) };
	SendPacket(Packet);
	UE_LOG(LogTemp, Log, TEXT("Ready called: Value=%s"), Value ? TEXT("true") : TEXT("false"));
}

void USTGameInstance::StartGame()
{
	Common::CSStartGame StartGamePacket{};
	auto Packet{ STSerializer::Serialize(StartGamePacket) };
	SendPacket(Packet);
	UE_LOG(LogTemp, Log, TEXT("StartGame called"));
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

void USTGameInstance::DevReady(bool Value)
{
	Ready(Value);
}

void USTGameInstance::DevStartGame()
{
	StartGame();
}

void USTGameInstance::SendPacket(const TArray<uint8>& Packet)
{
	NetworkManager->SendPacket(Packet);
}

void USTGameInstance::OnLevelLoaded(UWorld* LoadedWorld)
{
	if (LoadedWorld)
	{
		UE_LOG(LogTemp, Log, TEXT("Level loaded: %s"), *LoadedWorld->GetName());
		IsLoadingLevel = false;

		// 플레이어 객체 갱신
		DataManager->RefreshPlayers();

		// 호스트 플레이어 설정 시도
		DataManager->TrySetHostPlayer();
	}
}
