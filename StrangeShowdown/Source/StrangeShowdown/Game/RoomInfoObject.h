// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "RoomInfoObject.generated.h"

USTRUCT(BlueprintType)
struct FRoomInfo
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FString RoomName;

    UPROPERTY(BlueprintReadOnly)
    int32 CurrentPlayers;

    UPROPERTY(BlueprintReadOnly)
    int32 MaxPlayers = 5;

    UPROPERTY(BlueprintReadOnly)
    bool bHasPassword;
};

UCLASS()
class STRANGESHOWDOWN_API URoomInfoObject : public UObject
{
	GENERATED_BODY()
	
public:
    FRoomInfo RoomInfo;
};
