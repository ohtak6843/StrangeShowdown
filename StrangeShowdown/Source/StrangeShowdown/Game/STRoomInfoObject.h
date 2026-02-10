// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "STRoomInfoObject.generated.h"

USTRUCT(BlueprintType)
struct FRoomInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString RoomName;

	UPROPERTY(BlueprintReadWrite)
	int32 CurrentPlayers;

	UPROPERTY(BlueprintReadWrite)
	int32 MaxPlayers = 5;

	UPROPERTY(BlueprintReadWrite)
	bool bHasPassword;

	UPROPERTY(BlueprintReadWrite)
	FString Password;
};

UCLASS(Blueprintable)
class STRANGESHOWDOWN_API USTRoomInfoObject : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite)
	FRoomInfo RoomInfo;
};
