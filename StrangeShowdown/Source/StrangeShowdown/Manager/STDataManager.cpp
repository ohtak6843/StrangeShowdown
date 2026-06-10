// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/STDataManager.h"
#include "GameData/STTypes.h"
#include "Character/Player/STFieldPlayer.h"
#include "Character/STCharacter.h"
#include "Character/Player/STPlayerBase.h"
#include "Character/Player/STLobbyFieldPlayer.h"
#include "Character/Player/STLocalPlayer.h"
#include "Character/Player/STLobbyLocalPlayer.h"
#include "Game/STGameInstance.h"
#include "GameFramework/GameUserSettings.h"
#include "Controller/STLobbyController.h"
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


	auto PlayerInfoPtr{ PlayerInfoMap.Find(Packet.id) };
	
	// 플레이어가 존재하지 않음.
	if (nullptr == PlayerInfoPtr)
	{
		// playerinfo 생성
		FPlayerInfo PlayerInfo{
			.NickName = FString::Printf(TEXT("Player_%llu"), Packet.id),
			.PlayerID = Packet.id,
			.bIsHost = (Packet.id == HostID),
			.bIsReady = false,
			.Type = Packet.type
		};

		ASTPlayerBase* Player{ SpawnFieldPlayer(Transform, SpawnParams, PlayerInfo, Packet.type) };
		PlayerInfo.Player = Player;
		PlayerInfoMap.Add(Packet.id, PlayerInfo);

		UE_LOG(LogTemp, Log, TEXT("New Player %llu spawned, type : %d"), Packet.id, static_cast<int32>(Packet.type));
	}

	// 플레이어가 이미 존재함.
	else
	{
		// 기존 플레이어 객체를 제거한다.
		if (true == PlayerInfoPtr->Player.IsValid())
		{
			PlayerInfoPtr->Player->Destroy();
		}
		PlayerInfoPtr->Player.Reset();

		// 새로운 객체를 생성한다.
		ASTPlayerBase* Player{ SpawnFieldPlayer(Transform, SpawnParams, *PlayerInfoPtr, Packet.type) };
		PlayerInfoPtr->Player = Player;
		
	
		UE_LOG(LogTemp, Log, TEXT("Origin Player %llu spawned, type : %d"), Packet.id, static_cast<int32>(Packet.type));
	}
}

void USTDataManager::HandleDespawnPlayer(const Common::SCDespawnPlayer& Packet)
{
	auto Player{ GetPlayer(Packet.id) };
	if (true == Player.IsValid())
	{
		Player->Destroy();
		PlayerInfoMap.Find(Packet.id)->Player.Reset();

		// 유령 클라이언트의 딜레이를 고려한다? -> PC에 따라서 다 다를거니까
		// 1. 플레이어가 죽는 동시에 유령을 스폰한다.
		// 2, 플레이어가 죽어도 일정시간동안 기다렸다가 스폰한다.
		//    => 시간 좀 걸려  생각보다 까다로워. => Job을 통해서 넣어야돼. Job 시간이 안돼. JOb의 시간 지연 시스템 
		//    => 
	}
}

void USTDataManager::HandleMove(const Common::SCMovePlayer& Packet)
{
	auto Player{ GetPlayer(Packet.id) };
	if (true == Player.IsValid())
	{
		FVector Location{ Packet.pos.x, Packet.pos.y, Packet.pos.z };
		FRotator Rotation{ Packet.dir.x, Packet.dir.y, Packet.dir.z };
		Player->Move(Location, Rotation);
		Player->PlayerStateFlag = Packet.state;
	}
}

void USTDataManager::HandleCreateRoom(const Common::SCCreateRoom& Packet)
{
	if (true == Packet.success)
	{
		// 방을 만드는데 성공 했으면 방장이므로 호스트 플래그를 켜줌.
		bIsHost = true;

		// controller에 호스트 정보 전달

	}
}

void USTDataManager::HandleJoinRoom(const Common::SCJoinRoom& Packet)
{
	HostID = Packet.hostID;
	MyPlayerInfo.PlayerID = Packet.MyID;
	MyPlayerInfo.bIsHost = (MyPlayerInfo.PlayerID == HostID);
	bIsInGame = false;

	APlayerController* PlayerController{ UGameplayStatics::GetPlayerController(GetWorld(), 0) };
	ASTLobbyController* LobbyController{ Cast<ASTLobbyController>(PlayerController) };
	if (nullptr == LobbyController)
	{
		return;
	}
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
	auto Player{ GetPlayer(Packet.id) };
	if (Player.IsValid())
	{
		Player->PlayItemUseEffect(static_cast<EItemType>(Packet.itemType));
		UE_LOG(LogTemp, Log, TEXT("Player %llu used item: %d"), Packet.id, Packet.itemType);
	}

}

void USTDataManager::HandleStatusUpdate(const Common::SCStatusUpdate& Packet)
{
	// 본인 플레이어의 상태 업데이트
	if (Packet.id == MyPlayerInfo.PlayerID)
	{
		if (true == MyPlayerInfo.Player.IsValid())
		{
			MyPlayerInfo.Player->HandleStatusUpdate(Packet);
		}
		return;
	}
	// 다른 플레이어의 상태 업데이트
	auto Player{ GetPlayer(Packet.id) };
	if (true == Player.IsValid())
	{
		Player->HandleStatusUpdate(Packet);
	}
}

void USTDataManager::OnLevelChanged()
{
	RefreshPlayers();
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
	// 데이터는 유지한다.
	for (auto& [_, PlayerInfo] : PlayerInfoMap)
	{
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
	if (false == bIsInGame)
	{
		APlayerController* PlayerController{ UGameplayStatics::GetPlayerController(GetWorld(), 0) };
		ASTLobbyController* LobbyController{ Cast<ASTLobbyController>(PlayerController) };
		if (nullptr == LobbyController)
		{
			UE_LOG(LogTemp, Log, TEXT("PlayerController is not of type ASTLobbyController"));
			return;
		}
		LobbyController->InitData(HostID, MyPlayerInfo.PlayerID);
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
		auto* Player{ Cast<ASTLobbyLocalPlayer>(Pawn) };
		if (nullptr == Player)
		{
			UE_LOG(LogTemp, Warning, TEXT("PlayerPawn is not of type ASTLobbyLocalPlayer. Cannot get player."));
			return;
		}
		MyPlayerInfo.Player = Player;
	}

}

PlayerWeakPtr USTDataManager::GetPlayer(const uint64 PlayerID) const
{
	if (auto* InfoPtr{ PlayerInfoMap.Find(PlayerID) })
	{
		// FPlayerInfo 구조체 안에 있는 Player 포인터 참조
		auto Player{ InfoPtr->Player };

		return Player;
	}
	return nullptr;
}

ASTPlayerBase* USTDataManager::SpawnFieldPlayer(const FTransform& Transform, const FActorSpawnParameters& SpawnParams, const FPlayerInfo& PlayerInfo)
{
	ASTPlayerBase* player{ nullptr };
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

ASTPlayerBase* USTDataManager::SpawnFieldPlayer(const FTransform& Transform, const FActorSpawnParameters& SpawnParams, const FPlayerInfo& PlayerInfo, const Common::PlayerType PlayerType)
{
	ASTPlayerBase* player{ nullptr };
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
		auto* IngamePlayer = GetWorld()->SpawnActor<ASTFieldPlayer>(
			FieldPlayerClass,
			Transform,
			SpawnParams
		);
		IngamePlayer->Init(PlayerInfo);
		player = IngamePlayer;
		UE_LOG(LogTemp, Warning, TEXT("Ghost player type is not implemented yet. Spawning as regular player."));
	}
	break;

	default:
		break;
	}

	return player;
}



