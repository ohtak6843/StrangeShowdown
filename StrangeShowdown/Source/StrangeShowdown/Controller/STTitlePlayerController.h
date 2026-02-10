// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/STRoomInfoObject.h"
#include "GameFramework/PlayerController.h"
#include "STTitlePlayerController.generated.h"

/**
 * 
 */

UCLASS()
class STRANGESHOWDOWN_API ASTTitlePlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

public:
	// 서버에서 받은 룸 정보
	UPROPERTY(BlueprintReadWrite)
	USTRoomInfoObject* RoomInfo;

	// 서버에서 패킷을 받아 RoomInfo를 설정(NewRoom는 서버에서 받은 방 정보)
	void SetRoomInfo(USTRoomInfoObject* NewRoom);

	// RoomInfo가 설정된 후 방 정보를 추가하는 함수
	void AddRoomToList(USTRoomInfoObject* NewRoom);

	// 디버깅용 테스트 룸 생성
	UFUNCTION(BlueprintCallable)
	void CreateTestRoom();
};
