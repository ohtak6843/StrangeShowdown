// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/STItemUseEffect.h"
#include "STMeatEffect.generated.h"

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API USTMeatEffect : public USTItemUseEffect
{
	GENERATED_BODY()
	
public:
	USTMeatEffect();

	virtual bool Use(ASTLocalPlayer* User, USTItemDataAssetBase* ItemData) override;
};
