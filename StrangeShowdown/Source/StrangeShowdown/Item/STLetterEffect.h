// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/STItemUseEffect.h"
#include "STLetterEffect.generated.h"

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API USTLetterEffect : public USTItemUseEffect
{
	GENERATED_BODY()
	
public:
	USTLetterEffect();

	virtual bool Use(ASTLocalPlayer* User, USTItemDataAssetBase* ItemData) override;
};
