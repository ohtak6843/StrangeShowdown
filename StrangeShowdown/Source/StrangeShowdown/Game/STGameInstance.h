// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/RoomInfoObject.h"
#include "Engine/GameInstance.h"
#include "STGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API USTGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	TArray<URoomInfoObject*> RoomList;

	// TitlePlayerController에서 방 정보를 추가하는 함수
	void AddRoomToList(URoomInfoObject* NewRoom);
};
