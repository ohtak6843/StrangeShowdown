// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/STDataManager.h"
#include "GameData/STTypes.h"
#include "Character/Player/STFieldPlayer.h"
#include "Character/STCharacter.h"

#include "Game/STGameInstance.h"
#include "GameFramework/GameUserSettings.h"





void USTDataManager::init(TSubclassOf<ASTFieldPlayer> InOtherPlayerClass)
{
	OtherPlayerClass = InOtherPlayerClass;
}

void USTDataManager::HandleSpawn(const Common::SCSpawnObject& Packet)
{
	// UClass
	if (nullptr == OtherPlayerClass)
	{
		UE_LOG(LogTemp, Log, TEXT("OtherPlayerClass is NOT assigned!"));
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
		OtherPlayerClass,
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
	if (auto* InfoPtr{ PlayerInfoMap.Find(Packet.id) })
	{
		// FPlayerInfo 구조체 안에 있는 Player 포인터 참조
		ASTFieldPlayer* Player{ InfoPtr->Player };

		if (false == IsValid(Player))
		{
			UE_LOG(LogTemp, Warning, TEXT("Player with ID %d is not valid"), Packet.id);
			return;
		}

		FVector Location{ Packet.pos.x, Packet.pos.y, Packet.pos.z };
		FRotator Rotation{ Packet.dir.x, Packet.dir.y, Packet.dir.z };
		Player->Move(Location, Rotation);
		Player->PlayerStateFlag = Packet.state;
	}
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
			OtherPlayerClass,
			Transform,
			SpawnParams)
		};

		Elem.Value.Player = player;
	}
}
