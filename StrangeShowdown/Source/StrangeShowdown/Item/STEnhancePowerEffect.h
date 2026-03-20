// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/STItemUseEffect.h"
#include "STEnhancePowerEffect.generated.h"

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API USTEnhancePowerEffect : public USTItemUseEffect
{
	GENERATED_BODY()
	
public:
	USTEnhancePowerEffect();

	virtual bool Use(ASTLocalPlayer* User, USTItemDataAssetBase* ItemData) override;
};
