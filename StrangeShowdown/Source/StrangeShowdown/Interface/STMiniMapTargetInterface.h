// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "STMiniMapTargetInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class USTMiniMapTargetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class STRANGESHOWDOWN_API ISTMiniMapTargetInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MiniMap")
	FVector GetMiniMapLocation() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MiniMap")
	UTexture2D* GetMiniMapIcon() const;
};
