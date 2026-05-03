// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/STDataManager.h"
#include "GameData/STTypes.h"
#include "Character/Player/STFieldPlayer.h"
#include "Character/STCharacter.h"
#include "Character/Player/STPlayerBase.h"
#include "Character/Player/STLobbyFieldPlayer.h"
#include "Game/STGameInstance.h"
#include "GameFramework/GameUserSettings.h"
#include "Controller/STLobbyController.h"
#include "UI/Lobby/STLobbyHUD.h"

#include "Kismet/GameplayStatics.h"
#include "Interface/STControllerHUDInterface.h"




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

	// spawn player
	ASTPlayerBase* Player{ SpawnPlayer(Transform, SpawnParams, Packet.objectID)};

	// playerinfo 생성
	FPlayerInfo PlayerInfo{
		.Player = Player,
		.NickName = FString::Printf(TEXT("Player_%llu"), Packet.objectID),
		.PlayerID = Packet.objectID,
		.bIsHost = (Packet.objectID == HostID)
	};

	// playerinfoMap에 추가
	PlayerInfoMap.Add(Packet.objectID, PlayerInfo);
	
	UE_LOG(LogTemp, Log, TEXT("HandleSpawn: PlayerID=%llu"), Packet.objectID);
}

void USTDataManager::HandleMove(const Common::SCMovePlayer& Packet)
{
	if (auto* Player{ GetPlayer(Packet.id) })
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
		LobbyController->SetReady(Packet.id, Packet.ready);
	}
}

void USTDataManager::HandleStartGame(const Common::SCStartGame& Packet)
{
	if (true == Packet.start)
	{
		bIsInGame = true;
	}
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
		if (IsValid(PlayerInfo.Player) && false == PlayerInfo.Player->IsActorBeingDestroyed())
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
		auto* player{ SpawnPlayer(Transform, SpawnParams, PlayerInfo.PlayerID) };

		// playerinfo 업데이트
		PlayerInfo.Player = player;
	}
}

void USTDataManager::TrySetHostPlayer()
{
	if (true == bIsHost && false == bIsInGame)
	{
		APlayerController* PlayerController{ UGameplayStatics::GetPlayerController(GetWorld(), 0) };
		ASTLobbyController* LobbyController{ Cast<ASTLobbyController>(PlayerController) };
		if (nullptr == LobbyController)
		{
			UE_LOG(LogTemp, Log, TEXT("PlayerController is not of type ASTLobbyController"));
			return;
		}
		LobbyController->bIsRoomOwner = true;
		LobbyController->UpdateReadyText();
		UE_LOG(LogTemp, Log, TEXT("hostplayer set success"));
	}
	else
		UE_LOG(LogTemp, Log, TEXT("host set failed"));
}

ASTPlayerBase* USTDataManager::GetPlayer(const uint64 PlayerID) const
{
	if (auto* InfoPtr{ PlayerInfoMap.Find(PlayerID) })
	{
		// FPlayerInfo 구조체 안에 있는 Player 포인터 참조
		ASTPlayerBase* Player{ InfoPtr->Player };

		if (false == IsValid(Player))
		{
			UE_LOG(LogTemp, Warning, TEXT("Player with ID %llu is not valid"), PlayerID);
			return nullptr;
		}
		return Player;
	}
	return nullptr;
}

ASTPlayerBase* USTDataManager::SpawnPlayer(const FTransform& Transform, const FActorSpawnParameters& SpawnParams, const uint64 PlayerID)
{
	ASTPlayerBase* player{ nullptr };
	if (bIsInGame)
	{
		player = GetWorld()->SpawnActor<ASTFieldPlayer>(
			FieldPlayerClass,
			Transform,
			SpawnParams
		);
		UE_LOG(LogTemp, Log, TEXT("Field Player Spawned"));
	}
	else
	{
		auto* LobbyPlayer{ GetWorld()->SpawnActor<ASTLobbyFieldPlayer>(
			LobbyFieldPlayerClass,
			Transform,
			SpawnParams
		) };
		LobbyPlayer->Init(PlayerID);
		player = LobbyPlayer;
		UE_LOG(LogTemp, Log, TEXT("Lobby Field Player Spawned"));
	}
	return player;
}

