// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/STItemUseEffect.h"
#include "STWheelEffect.generated.h"

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API USTWheelEffect : public USTItemUseEffect
{
	GENERATED_BODY()
	
public:
	USTWheelEffect();

	virtual bool Use(ASTLocalPlayer* User, USTItemDataAssetBase* ItemData) override;
};
