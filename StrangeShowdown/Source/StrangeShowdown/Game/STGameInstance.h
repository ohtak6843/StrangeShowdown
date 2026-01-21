// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/RoomInfoObject.h"
#include "Engine/GameInstance.h"
#include "STGameInstance.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRoomListUpdated);

UCLASS()
class STRANGESHOWDOWN_API USTGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	// 방 목록이 업데이트되었음을 알리는 델리게이트
	UPROPERTY(BlueprintAssignable)
	FOnRoomListUpdated OnRoomListUpdated;

	// 방 목록을 저장하는 배열
	UPROPERTY(BlueprintReadWrite)
	TArray<URoomInfoObject*> RoomList;

	// 새로운 방을 추가
	void AddRoom(URoomInfoObject* NewRoom);
};
