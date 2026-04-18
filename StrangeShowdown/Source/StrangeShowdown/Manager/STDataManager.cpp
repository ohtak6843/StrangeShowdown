// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/STDataManager.h"
#include "GameData/STTypes.h"
#include "Character/Player/STFieldPlayer.h"
#include "Character/STCharacter.h"

#include "Game/STGameInstance.h"
#include "GameFramework/GameUserSettings.h"




void USTDataManager::HandleSpawn(TSubclassOf<ASTFieldPlayer> OtherPlayerClass , const Common::SCSpawnObject& Packet)
{
	// UClass
	if (nullptr == OtherPlayerClass)
	{
		UE_LOG(LogTemp, Log, TEXT("OtherPlayerClass is NOT assigned!"));
		return;
	}

	// transform
	FTransform transform{ FTransform::Identity };

	transform.SetLocation(FVector(Packet.pos.x, Packet.pos.y, Packet.pos.z));
	transform.SetRotation(FQuat::Identity);

	// spawn param
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// spawn player
	auto* player{ GetWorld()->SpawnActor<ASTFieldPlayer>(
		OtherPlayerClass,
		transform,
		SpawnParams
	) };

	// playermap¿¡ Ãß°¡
	PlayerMap.Add(Packet.objectID, player);
	
	UE_LOG(LogTemp, Log, TEXT("HandleSpawn: PlayerID=%llu"), Packet.objectID);
}

void USTDataManager::HandleMove(const Common::SCMovePlayer& Packet)
{
	if (ASTFieldPlayer * *PlayerPtr{ PlayerMap.Find(Packet.id) })
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

void USTDataManager::ChangeMap(const FString& MapName)
{
}
