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


void USTGameInstance::HandleSpawnPlayer(const Common::SCSpawnPlayer& Packet)
{
	// player bp class
	DataManager->HandleSpawnPlayer(Packet);

}

void USTGameInstance::HandleDespawnPlayer(const Common::SCDespawnPlayer& Packet)
{
	DataManager->HandleDespawnPlayer(Packet);
}

void USTGameInstance::HandleSpawnObject(const Common::SCSpawnObject& Packet)
{
	DataManager->HandleSpawnObject(Packet);
}

void USTGameInstance::HandleDespawnObject(const Common::SCDespawnObject& Packet)
{
	DataManager->HandleDespawnObject(Packet);
}

void USTGameInstance::HandleMove(const Common::SCMovePlayer& Packet)
{
	DataManager->HandleMove(Packet);
}

void USTGameInstance::HandleCreateRoom(const Common::SCCreateRoom& Packet)
{
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

//void USTGameInstance::HandleDamage(const Common::SCDamage& Packet)
//{
//	// 여기부터 작업하시오
//	DataManager->HandleDamage(Packet);
//	UE_LOG(LogTemp, Log, TEXT("Damage: TargetID=%d, Damage=%.2f"), Packet.targetID, Packet.damage);
//}

void USTGameInstance::HandleStatusUpdate(const Common::SCStatusUpdate& Packet)
{
	//DataManager->HandleStatusUpdate(Packet);
	// UE_LOG(LogTemp, Log, TEXT("Status Update: ID=%d, Health=%.2f, Stamina=%.2f"), Packet.id, Packet.health, Packet.stamina);
	DataManager->HandleStatusUpdate(Packet);
	UE_LOG(LogTemp, Log, TEXT("Status Update: PlayerID=%d, HP=%.2f, Stamina=%.2f, Bullet=%.2f, Gold=%.2f, Armor=%.2f"),
		Packet.id,
		Packet.hp,
		Packet.stamina,
		Packet.bullet,
		Packet.gold,
		Packet.armor
	);
	
}

void USTGameInstance::HandleSetTurn(const Common::SCSetTurn& Packet)
{
	DataManager->HandleSetTurn(Packet);
	UE_LOG(LogTemp, Log, TEXT("Set Turn: Turn=%d"), Packet.turn);
}


void USTGameInstance::HandleUseItem(const Common::SCUseItem& Packet)
{
	DataManager->HandleUseItem(Packet);
	UE_LOG(LogTemp, Log, TEXT("Use ID=%d, Use Item: target ID=%d, ItemID=%d"), Packet.id, Packet.targetID, static_cast<uint32>(Packet.itemType));
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
	//DataManager->SetLoadingLevel(true);

	NetworkManager->Pause();

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

void USTGameInstance::Ready(const bool Value)
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

void USTGameInstance::UseItem(const uint64 TargetID, const Common::ItemType ItemType)
{
	Common::CSUseItem UseItemPacket{ TargetID, ItemType };
	auto Packet{ STSerializer::Serialize(UseItemPacket) };
	SendPacket(Packet);
	UE_LOG(LogTemp, Log, TEXT("UseItem called: ItemType=%d"), static_cast<uint32>(ItemType));
}

void USTGameInstance::PickUpItem(const Common::ItemType ItemID)
{
	Common::CSPickItem PickUpItemPacket{ ItemID };
	auto Packet{ STSerializer::Serialize(PickUpItemPacket) };
	SendPacket(Packet);
	UE_LOG(LogTemp, Log, TEXT("PickUpItem called: ItemID=%d"), static_cast<uint32>(ItemID));
}

void USTGameInstance::MovePlayer(const FVector Location, const FRotator Rotator, const uint8 Flag)
{
	Vec3f LocationVec{
		static_cast<float>(Location.X), 
		static_cast<float>(Location.Y), 
		static_cast<float>(Location.Z)
	};

	Vec3f RotatorVec{
		static_cast<float>(Rotator.Pitch),
		static_cast<float>(Rotator.Yaw),
		static_cast<float>(Rotator.Roll)
	};
	Common::CSMovePlayer MovePacket{ LocationVec, RotatorVec, Flag };
	auto Packet{ STSerializer::Serialize(MovePacket) };
	SendPacket(Packet);
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

void USTGameInstance::DevUseItem(uint32 TargetID, uint32 ItemType)
{
	UseItem(static_cast<uint64>(TargetID), static_cast<Common::ItemType>(ItemType));
}

void USTGameInstance::SendPacket(const TArray<uint8>& Packet)
{
	NetworkManager->SendPacket(Packet);
}

void USTGameInstance::OnLevelLoaded(UWorld* LoadedWorld)
{
	if (false == IsValid(LoadedWorld) ||
		false == LoadedWorld->IsGameWorld() ||
		LoadedWorld != GetWorld())
	{
		UE_LOG(LogTemp, Warning, TEXT("OnLevelLoaded: LoadedWorld is invalid"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Level loaded: %s"), *LoadedWorld->GetName());

	// 플레이어 객체 갱신
	DataManager->OnLevelChanged();

	// NetworkManager 재개
	NetworkManager->Resume();
}
