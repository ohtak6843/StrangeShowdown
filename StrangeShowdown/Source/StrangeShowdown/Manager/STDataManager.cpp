// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/STDataManager.h"
#include "GameData/STTypes.h"
#include "Character/Player/STFieldPlayer.h"
#include "Character/STCharacter.h"
#include "Character/Player/STPlayerBase.h"

#include "Game/STGameInstance.h"
#include "GameFramework/GameUserSettings.h"





void USTDataManager::HandleSpawn(const Common::SCSpawnObject& Packet)
{
	// UClass
	if (nullptr == FieldPlayerClass)
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
	auto* player{ GetWorld()->SpawnActor<ASTFieldPlayer>(
		FieldPlayerClass,
		Transform,
		SpawnParams
	) };

	// playerinfo 생성
	FPlayerInfo PlayerInfo{
		.Player = player,
		.NickName = FString::Printf(TEXT("Player_%llu"), Packet.objectID),
		.PlayerID = Packet.objectID
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
	// 방을 만드는데 성공 했으면 방장이므로 호스트 플래그를 켜줌.
	if (true == Packet.success)
	{
		bIsHost = true;
	}
}

void USTDataManager::HandleJoinRoom(const Common::SCJoinRoom& Packet)
{
	HostID = Packet.hostID;
	bIsInGame = false;
}

void USTDataManager::RefreshPlayers()
{
	for(auto& Elem : PlayerInfoMap)
	{
		if (IsValid(Elem.Value.Player) && false == Elem.Value.Player->IsActorBeingDestroyed())
		{
			Elem.Value.Player->Destroy();
		}

		FTransform Transform{ FTransform::Identity };

		Transform.SetLocation(FVector{});
		Transform.SetRotation(FQuat::Identity);

		// spawn param
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;


		auto* player{ GetWorld()->SpawnActor<ASTFieldPlayer>(
			FieldPlayerClass,
			Transform,
			SpawnParams)
		};

		Elem.Value.Player = player;
	}
}

ASTFieldPlayer* USTDataManager::GetPlayer(const uint64 PlayerID) const
{
	if (auto* InfoPtr{ PlayerInfoMap.Find(PlayerID) })
	{
		// FPlayerInfo 구조체 안에 있는 Player 포인터 참조
		ASTFieldPlayer* Player{ InfoPtr->Player };

		if (false == IsValid(Player))
		{
			UE_LOG(LogTemp, Warning, TEXT("Player with ID %llu is not valid"), PlayerID);
			return nullptr;
		}
		return Player;
	}
	return nullptr;
}

