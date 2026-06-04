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



void USTDataManager::HandleSpawn(const Common::SCSpawnObject& Packet)
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

	// transform
	FTransform Transform{ FTransform::Identity };

	Transform.SetLocation(FVector(Packet.pos.x, Packet.pos.y, Packet.pos.z));
	Transform.SetRotation(FQuat::Identity);

	// spawn param
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// playerinfo 생성
	FPlayerInfo PlayerInfo{
		.NickName = FString::Printf(TEXT("Player_%llu"), Packet.objectID),
		.PlayerID = Packet.objectID,
		.bIsHost = (Packet.objectID == HostID)
	};

	// spawn player
	ASTPlayerBase* Player{ SpawnFieldPlayer(Transform, SpawnParams, PlayerInfo) };

	PlayerInfo.Player = Player;

	// playerinfoMap에 추가
	PlayerInfoMap.Add(Packet.objectID, PlayerInfo);
	
	UE_LOG(LogTemp, Log, TEXT("HandleSpawn: PlayerID=%llu"), Packet.objectID);
}

void USTDataManager::HandleMove(const Common::SCMovePlayer& Packet)
{
	auto Player{ GetPlayer(Packet.id) };
	if (Player.IsValid())
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

void USTDataManager::HandleDamage(const Common::SCDamage& Packet)
{
	// 본인 플레이어가 데미지를 입었을 경우
	if (Packet.targetID == MyPlayerInfo.PlayerID)
	{
		if (MyPlayerInfo.Player.IsValid())
		{
			MyPlayerInfo.Player->TakeDamage(Packet.damage, FDamageEvent(UDamageType::StaticClass()), nullptr, nullptr);
		}
		UE_LOG(LogTemp, Log, TEXT("My Player took damage: %f"), Packet.damage);
		return;
	}

	// 다른 플레이어가 데미지를 입었을 경우
	auto Player{ GetPlayer(Packet.targetID) };
	if (true == Player.IsValid())
	{
		Player->TakeDamage(Packet.damage, FDamageEvent(UDamageType::StaticClass()), nullptr, nullptr);
		UE_LOG(LogTemp, Log, TEXT("Player %llu took damage: %f"), Packet.targetID, Packet.damage);
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
		if (MyPlayerInfo.Player.IsValid())
		{
		}
		return;
	}
	// 다른 플레이어의 상태 업데이트
	if (auto* PlayerInfoPtr{ PlayerInfoMap.Find(Packet.id) })
	{
		if (PlayerInfoPtr->Player.IsValid())
		{
		//	PlayerInfoPtr->Player->UpdateStatus(Packet.hp, Packet.stamina, Packet.bullet, Packet.gold, Packet.armor);
		}
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

