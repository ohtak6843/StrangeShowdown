// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/RoomInfoObject.h"
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
	UPROPERTY(BlueprintReadWrite)
	TArray<URoomInfoObject*> RoomList;

	// 서버에서 받아서 방 정보를 추가하는 함수
	void AddRoomToList(URoomInfoObject* NewRoom);
};
