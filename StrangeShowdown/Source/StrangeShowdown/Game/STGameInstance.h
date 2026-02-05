// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/STRoomInfoObject.h"
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

public:
	// 방 목록을 저장하는 배열
	UPROPERTY(BlueprintReadWrite)
	TArray<USTRoomInfoObject*> RoomList;

	// 해상도
	UPROPERTY(BlueprintReadWrite)
	FIntPoint ScreenResolution;

	// 창모드
	UPROPERTY(BlueprintReadWrite)
	TEnumAsByte<EWindowMode::Type> WindowMode = EWindowMode::Windowed;
	
	// 볼륨 설정
	UPROPERTY(BlueprintReadWrite)
	float MasterVolume = 1.0f;

	UPROPERTY(BlueprintReadWrite)
	float BGMVolume = 1.0f;

	UPROPERTY(BlueprintReadWrite)
	float SFXVolume = 1.0f;

public:
	virtual void Init() override;

	// 새로운 방을 추가
	void AddRoom(USTRoomInfoObject* NewRoom);
};
