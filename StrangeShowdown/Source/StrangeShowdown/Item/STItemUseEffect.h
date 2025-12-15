// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "STItemUseEffect.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class STRANGESHOWDOWN_API USTItemUseEffect : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "Item")
	void Use(ASTLocalPlayer* User);

	virtual void Use_Implementation(ASTLocalPlayer* User);
};
