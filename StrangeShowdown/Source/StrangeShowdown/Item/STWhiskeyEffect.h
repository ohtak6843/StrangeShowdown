// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/STItemUseEffect.h"
#include "STWhiskeyEffect.generated.h"

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API USTWhiskeyEffect : public USTItemUseEffect
{
	GENERATED_BODY()
	
public:
		USTWhiskeyEffect();

		virtual bool Use(ASTLocalPlayer* User, USTItemDataAssetBase* ItemData) override;
};
