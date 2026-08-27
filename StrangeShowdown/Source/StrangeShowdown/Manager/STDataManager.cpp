// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/STDataManager.h"
#include "GameData/STTypes.h"
#include "Character/Player/STFieldPlayer.h"
#include "Character/STCharacter.h"
#include "Character/Player/STPlayerBase.h"
#include "Character/Player/STLobbyFieldPlayer.h"
#include "Character/Player/STLocalPlayer.h"
#include "Character/Player/STLobbyLocalPlayer.h"
#include "Character/Ghost/STFieldGhost.h"
#include "Character/Ghost/STLocalGhost.h"
#include "Character/Sheriff/STFieldSheriff.h"
#include "Character/Sheriff/STLocalSheriff.h"

#include "Game/STGameInstance.h"
#include "GameFramework/GameUserSettings.h"
#include "Controller/STLobbyController.h"
#include "Controller/STPlayerController.h"
#include "UI/Lobby/STLobbyHUD.h"

#include "Kismet/GameplayStatics.h"
#include "Interface/STControllerHUDInterface.h"

#include "Engine/DamageEvents.h"



void USTDataManager::HandleSpawnPlayer(const Common::SCSpawnPlayer& Packet)
{
	// 타입에 따라 생성할 플레이어 클래스 결정

	
	// blueprint class 예외 처리
	if (false == bIsInGame && nullptr == LobbyFieldPlayerClass)
	{
		UE_LOG(LogTemp, Log, TEXT("LobbyFieldPlayerClass is NOT assigned!"));
		return;
	}

	if (true == bIsInGame && nullptr == FieldPlayerClass)
	{
		UE_LOG(LogTemp, Log, TEXT("FieldPlayerClass is NOT assigned!"));
		return;
	}
	
	// transform
	FTransform Transform{ FTransform::Identity };

	Transform.SetLocation(FVector(Packet.pos.x, Packet.pos.y, Packet.pos.z));
	Transform.SetRotation(FQuat::Identity);

	// spawn param
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// 본인 플레이어 처리
	// todo: 일단 냅둠
	if (Packet.id == MyPlayerInfo.ID)
	{
		if (auto* PlayerPtr{ Cast<ASTCharacter>(MyPlayerInfo.Player.Get()) })
		{
			// 받아온 Transform 위치로 캐릭터를 순간이동
			PlayerPtr->Teleport(Transform);

			UE_LOG(LogTemp, Log, TEXT("My Player %llu teleported to new location, type : %d"), Packet.id, static_cast<int32>(Packet.type));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("My Player %llu exists but is not completely valid or not an ASTCharacter."), Packet.id);
		}

		if (MyPlayerInfo.Type != Packet.type)
		{
			// 타입이 다르면 캐릭터를 교체
			ChangeType(Packet.type);

			MyPlayerInfo.Type = Packet.type;
		}
		
		UE_LOG(LogTemp, Log, TEXT("My Player %llu spawned, type : %d"), Packet.id, static_cast<int32>(Packet.type));
		
		return;
	}

	

	// 다른 플레이어 스폰 처리
	auto PlayerInfoPtr{ PlayerInfoMap.Find(Packet.id) };
	
	// 기존 플레이어 정보가 존재하지 않음.
	if (nullptr == PlayerInfoPtr)
	{
		// playerinfo 생성
		FPlayerInfo PlayerInfo{
			.NickName = FString::Printf(TEXT("Player_%llu"), Packet.id),
			.ID = Packet.id,
			.bIsHost = (Packet.id == HostID),
			.bIsReady = false,
			.Type = Packet.type
		};

		auto* Player{ SpawnFieldPlayer(Transform, SpawnParams, PlayerInfo, Packet.type) };
		PlayerInfo.Player = Player;
		PlayerInfoMap.Add(Packet.id, PlayerInfo);

		UE_LOG(LogTemp, Log, TEXT("New Player %llu spawned, type : %d"), Packet.id, static_cast<int32>(Packet.type));
	}

	// 플레이어가 이미 존재함.
	else
	{
		// 기존 플레이어 객체가 있으면 제거한다.
		if (auto PlayerPtr{ PlayerInfoPtr->Player.Get() }; PlayerPtr != nullptr)
		{
			PlayerPtr->Destroy();
			PlayerInfoPtr->Player.Reset();
		}

		// 새로운 객체를 생성한다.
		auto* Player{ SpawnFieldPlayer(Transform, SpawnParams, *PlayerInfoPtr, Packet.type) };
		PlayerInfoPtr->Player = Player;
		
		// 기존 정보를 업데이트한다.
		PlayerInfoPtr->Type = Packet.type;
	
		UE_LOG(LogTemp, Log, TEXT("Origin Player %llu spawned, type : %d"), Packet.id, static_cast<int32>(Packet.type));
	}
}

void USTDataManager::HandleDespawnPlayer(const Common::SCDespawnPlayer& Packet)
{
	auto PlayerInfoPtr{ GetPlayerInfo(Packet.id) };
	if (nullptr == PlayerInfoPtr)
	{
		return;
	}
	auto PlayerPtr{ PlayerInfoPtr->Player.Get()};
	if (nullptr == PlayerPtr)
	{
		return;
	}

	PlayerPtr->Destroy();
	PlayerInfoPtr->Player.Reset();

		// 유령 클라이언트의 딜레이를 고려한다? -> PC에 따라서 다 다를거니까
		// 1. 플레이어가 죽는 동시에 유령을 스폰한다.
		// 2, 플레이어가 죽어도 일정시간동안 기다렸다가 스폰한다.
		//    => 시간 좀 걸려  생각보다 까다로워. => Job을 통해서 넣어야돼. Job 시간이 안돼. JOb의 시간 지연 시스템 
		//    => 
	

}

void USTDataManager::HandleSpawnObject(const Common::SCSpawnObject& Packet)
{
	// transform
	FTransform Transform{ FTransform::Identity };

	Transform.SetLocation(FVector(Packet.pos.x, Packet.pos.y, Packet.pos.z));
	Transform.SetRotation(FQuat::Identity);

	// spawn param
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;


	auto ObjectInfoPtr{ ObjectInfoMap.Find(Packet.id) };

	if (nullptr != ObjectInfoPtr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Object %llu already exists, type : %d"), Packet.id, static_cast<int32>(Packet.type));
		return;
	}

	switch (Packet.type)
	{
	case Common::ObjectType::Sheriff:
	{
		auto* SheriffPtr{ GetWorld()->SpawnActor<ASTFieldSheriff>(
			LobbyFieldPlayerClass,
			Transform,
			SpawnParams
		) };
		FObjectInfo ObjectInfo{
			.Object = SheriffPtr,
			.ID = Packet.id,
			.Type = Packet.type,
		};
		ObjectInfoMap.Add(Packet.id, ObjectInfo);
		UE_LOG(LogTemp, Log, TEXT("New Object %llu spawned, type : %d"), Packet.id, static_cast<int32>(Packet.type));
		return;
	}
	break;
	default:
		break;
	}

}

void USTDataManager::HandleDespawnObject(const Common::SCDespawnObject& Packet)
{
	auto PlayerInfoPtr{ GetPlayerInfo(Packet.id) };
	if (nullptr == PlayerInfoPtr)
	{
		return;
	}
	auto PlayerPtr{ PlayerInfoPtr->Player.Get() };
	if (nullptr == PlayerPtr)
	{
		return;
	}

	PlayerPtr->Destroy();
	PlayerInfoPtr->Player.Reset();
}


void USTDataManager::HandleMove(const Common::SCMovePlayer& Packet)
{
	// 예외 처리
	auto PlayerInfoPtr{ GetPlayerInfo(Packet.id) };
	if (nullptr == PlayerInfoPtr)
	{
		UE_LOG(LogTemp, Log, TEXT("PlayerInfoPtr is nullptr for ID: %llu"), Packet.id);
		return;
	}

	auto PlayerPtr{ PlayerInfoPtr->Player.Get() };
	if (nullptr == PlayerPtr)
	{
		UE_LOG(LogTemp, Log, TEXT("PlayerPtr is nullptr for ID: %llu"), Packet.id);
		return;
	}

	// 패킷 처리
	FVector Location{ Packet.pos.x, Packet.pos.y, Packet.pos.z };
	FRotator Rotation{ Packet.dir.x, Packet.dir.y, Packet.dir.z };

	if (Common::PlayerType::Player == PlayerInfoPtr->Type ||
		Common::PlayerType::LobbyPlayer == PlayerInfoPtr->Type)
	{
		auto* PlayerBasePtr{ Cast<ASTPlayerBase>(PlayerPtr) };
		if (nullptr == PlayerBasePtr)
		{
			return;
		}
		PlayerBasePtr->PlayerStateFlag = Packet.state;
	}
	PlayerPtr->Move(Location, Rotation);
}

void USTDataManager::HandleCreateRoom(const Common::SCCreateRoom& Packet)
{
	if (true == Packet.success)
	{
		bIsHost = true;
	}
}

void USTDataManager::HandleJoinRoom(const Common::SCJoinRoom& Packet)
{
	// 정보 저장
	HostID = Packet.hostID;
	MyPlayerInfo.ID = Packet.MyID;
	MyPlayerInfo.bIsHost = (MyPlayerInfo.ID == HostID);
	PlayerInfoMap.Add(Packet.MyID, MyPlayerInfo);
	bIsInGame = false;
}

void USTDataManager::HandleReady(const Common::SCReady& Packet)
{
	if (true == bIsInGame)
	{
		return;
	}

	auto* PlayerController{ UGameplayStatics::GetPlayerController(GetWorld(), 0) };
	ASTLobbyController* LobbyController{ Cast<ASTLobbyController>(PlayerController) };
	if (false == IsValid(LobbyController))
	{
		return;
	}

	if (auto* PlayerInfoPtr{ PlayerInfoMap.Find(Packet.id) })
	{
		PlayerInfoPtr->bIsReady = Packet.ready;
		LobbyController->SetOtherPlayerReady(Packet.id, Packet.ready);
	}
}

void USTDataManager::HandleStartGame(const Common::SCStartGame& Packet)
{
	if (true == Packet.start)
	{
		bIsInGame = true;
	}
}

void USTDataManager::HandleUseItem(const Common::SCUseItem& Packet)
{
	// 예외처리
	auto PlayerInfoPtr{ GetPlayerInfo(Packet.id) };
	if (nullptr == PlayerInfoPtr)
	{
		return;
	}

	auto PlayerPtr{ PlayerInfoPtr->Player.Get() };
	if (nullptr == PlayerPtr)
	{
		return;
	}

	
	if (Common::PlayerType::Player == PlayerInfoPtr->Type)
	{
		auto* PlayerBasePtr{ Cast<ASTPlayerBase>(PlayerPtr) };
		if (nullptr == PlayerBasePtr)
		{
			return;
		}

		PlayerBasePtr->PlayItemUseEffect(static_cast<EItemType>(Packet.itemType));
	}
	

}

void USTDataManager::HandleStatusUpdate(const Common::SCStatusUpdate& Packet)
{
	auto PlayerInfoPtr{ GetPlayerInfo(Packet.id) };
	if (nullptr == PlayerInfoPtr)
	{
		return;
	}

	auto PlayerPtr{ PlayerInfoPtr->Player.Get() };
	if (nullptr == PlayerPtr)
	{
		return;
	}
	
	auto* PlayerBasePtr{ Cast<ASTPlayerBase>(PlayerPtr) };
	if (nullptr == PlayerBasePtr)
	{
		return;
	}
	PlayerBasePtr->HandleStatusUpdate(Packet);
}

void USTDataManager::HandleSetTurn(const Common::SCSetTurn& Packet)
{
	// 현재 월드를 가져옵니다.
	if (UWorld* World{ GetWorld() }; IsValid(World))
	{
		// 첫 번째 로컬 플레이어의 컨트롤러를 가져와 ASTPlayerController로 캐스팅합니다.
		if (ASTPlayerController* PC{ Cast<ASTPlayerController>(World->GetFirstPlayerController()) }; IsValid(PC))
		{
			// 패킷 데이터 혹은 필요한 인자를 넘겨주어 SetTurn을 호출합니다.
			PC->SetTimer(Packet.turn, Packet.time);
		}
	}
}
void USTDataManager::OnLevelChanged()
{
	// RefreshPlayers();
	InitController();
	GetMyPlayer();
}

void USTDataManager::RefreshPlayers()
{
	// blueprint class 예외 처리
	if (false == bIsInGame && nullptr == LobbyFieldPlayerClass)
	{
		UE_LOG(LogTemp, Log, TEXT("LobbyFieldPlayerClass is NOT assigned!"));
		return;
	}

	if (true == bIsInGame && nullptr == FieldPlayerClass)
	{
		UE_LOG(LogTemp, Log, TEXT("FieldPlayerClass is NOT assigned!"));
		return;
	}

	// 기존 플레이어 객체들을 제거하고 새 객체를 생성하는 로직.
	// 본인 플레이어는 생성하지 않는다.
	// 데이터는 유지한다.
	for (auto& [_, PlayerInfo] : PlayerInfoMap)
	{
		if (PlayerInfo.ID == MyPlayerInfo.ID)
		{
			continue;
		}

		if (PlayerInfo.Player.IsValid())
		{
			PlayerInfo.Player->Destroy();
		}

		// transform
		FTransform Transform{ FTransform::Identity };
		Transform.SetLocation(FVector{});
		Transform.SetRotation(FQuat::Identity);

		// spawn param
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		// spawn player
		auto* player{ SpawnFieldPlayer(Transform, SpawnParams, PlayerInfo) };

		// playerinfo 업데이트
		PlayerInfo.Player = player;
	}
}

void USTDataManager::InitController()
{
	// 로비 상태일 때만 설정한다.
	// todo: 중간 난입도 생각해야 함.
	if (false == bIsInGame)
	{
		APlayerController* PlayerController{ UGameplayStatics::GetPlayerController(GetWorld(), 0) };
		ASTLobbyController* LobbyController{ Cast<ASTLobbyController>(PlayerController) };
		if (nullptr == LobbyController)
		{
			UE_LOG(LogTemp, Log, TEXT("PlayerController is not of type ASTLobbyController"));
			return;
		}
		LobbyController->InitData(HostID, MyPlayerInfo.ID);
		UE_LOG(LogTemp, Log, TEXT("hostplayer set success"));
	}
}

void USTDataManager::GetMyPlayer()
{
	// 플레이어 폰 가져오기
	APawn* Pawn{ UGameplayStatics::GetPlayerPawn(GetWorld(), 0) };
	if (nullptr == Pawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerPawn is null. Cannot get player."));
		return;
	}

	// 플레이어 객체가 게임 중인지 로비 중인지에 따라 캐스팅을 다르게 함.
	if (true == bIsInGame)
	{
		// todo: hard codding
		auto* Player{ Cast<ASTLocalPlayer>(Pawn) };
		if (nullptr == Player)
		{
			UE_LOG(LogTemp, Warning, TEXT("PlayerPawn is not of type ASTLocalPlayer. Cannot get player."));
			return;
		}
		MyPlayerInfo.Player = Player;
		Player->Init();
	}
	else
	{
		// todo: hard codding
		auto* Player{ Cast<ASTLobbyLocalPlayer>(Pawn) };
		if (nullptr == Player)
		{
			UE_LOG(LogTemp, Warning, TEXT("PlayerPawn is not of type ASTLobbyLocalPlayer. Cannot get player."));
			return;
		}
		MyPlayerInfo.Player = Player;
	}
}

FPlayerInfo* USTDataManager::GetPlayerInfo(const uint64 ID)
{
	if (auto* InfoPtr{ PlayerInfoMap.Find(ID) })
	{
		return InfoPtr;
	}
	return nullptr;
}

ASTCharacter* USTDataManager::SpawnFieldPlayer(const FTransform& Transform, const FActorSpawnParameters& SpawnParams, const FPlayerInfo& PlayerInfo)
{
	ASTCharacter* player{ nullptr };
	if (false == IsValid(GetWorld()))
	{
		UE_LOG(LogTemp, Warning, TEXT("World is not valid. Cannot spawn player."));
		return nullptr;
	}

	if (true == bIsInGame)
	{
		auto* IngamePlayer = GetWorld()->SpawnActor<ASTFieldPlayer>(
			FieldPlayerClass,
			Transform,
			SpawnParams
		);
		IngamePlayer->Init(PlayerInfo);
		player = IngamePlayer;
		UE_LOG(LogTemp, Log, TEXT("Field Player Spawned"));
	}
	else
	{
		auto* LobbyPlayer{ GetWorld()->SpawnActor<ASTLobbyFieldPlayer>(
			LobbyFieldPlayerClass,
			Transform,
			SpawnParams
		) };
		LobbyPlayer->Init(PlayerInfo);
		player = LobbyPlayer;
		UE_LOG(LogTemp, Log, TEXT("Lobby Field Player Spawned"));
	}
	return player;
}

ASTCharacter* USTDataManager::SpawnFieldPlayer(const FTransform& Transform, const FActorSpawnParameters& SpawnParams, const FPlayerInfo& PlayerInfo, const Common::PlayerType PlayerType)
{
	ASTCharacter* player{ nullptr };
	if (false == IsValid(GetWorld()))
	{
		UE_LOG(LogTemp, Warning, TEXT("World is not valid. Cannot spawn player."));
		return nullptr;
	}

	switch (PlayerType)
	{
	case Common::PlayerType::LobbyPlayer:
	{
		auto* LobbyPlayer{ GetWorld()->SpawnActor<ASTLobbyFieldPlayer>(
			LobbyFieldPlayerClass,
			Transform,
			SpawnParams
		) };
		LobbyPlayer->Init(PlayerInfo);
		player = LobbyPlayer;
		UE_LOG(LogTemp, Log, TEXT("Lobby Field Player Spawned"));
	}
	break;
	case Common::PlayerType::Player:
	{
		auto* IngamePlayer = GetWorld()->SpawnActor<ASTFieldPlayer>(
			FieldPlayerClass,
			Transform,
			SpawnParams
		);
		IngamePlayer->Init(PlayerInfo);
		player = IngamePlayer;
		UE_LOG(LogTemp, Log, TEXT("Field Player Spawned"));
	}
	break;
	case Common::PlayerType::Ghost:
	{
		auto* Ghost = GetWorld()->SpawnActor<ASTFieldGhost>(
			FieldGhostClass,
			Transform,
			SpawnParams
		);
		// Ghost->Init(PlayerInfo);
		player = Ghost;
	}
	break;

	default:
		break;
	}

	return player;
}

void USTDataManager::ChangeType(const Common::PlayerType NewType)
{
    // 현재 타입이 Player이고, 변경하려는 타입이 Ghost인지 확인
    if (Common::PlayerType::Player == MyPlayerInfo.Type && Common::PlayerType::Ghost == NewType)
    {
        // 본인 플레이어를 ASTLocalPlayer로 안전하게 캐스팅
        if (ASTLocalPlayer* LocalPlayer{ Cast<ASTLocalPlayer>(MyPlayerInfo.Player.Get()) })
        {
            // ChangeToGhost()를 실행하고 반환값을 다시 MyPlayerInfo.Player에 저장
            MyPlayerInfo.Player = LocalPlayer->ChangeToGhost();
            
            // 타입 정보 업데이트
            MyPlayerInfo.Type = NewType;
            
            UE_LOG(LogTemp, Log, TEXT("Player successfully changed to Ghost."));
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to cast MyPlayerInfo.Player to ASTLocalPlayer."));
        }
    }
    
}



