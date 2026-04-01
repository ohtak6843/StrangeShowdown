// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/STItemUseEffect.h"
#include "STHelmetEffect.generated.h"

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API USTHelmetEffect : public USTItemUseEffect
{
	GENERATED_BODY()
	
public:
	USTHelmetEffect();

	virtual bool Use(ASTLocalPlayer* User, USTItemDataAssetBase* ItemData) override;
};
