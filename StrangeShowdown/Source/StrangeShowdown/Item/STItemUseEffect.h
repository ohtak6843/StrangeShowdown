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
	USTItemUseEffect();

	virtual bool Use(ASTLocalPlayer* User, USTItemDataAssetBase* ItemData);

	UFUNCTION(BlueprintCallable, Category = "Item")
	bool CanUse(ASTLocalPlayer* User, USTItemDataAssetBase* ItemData);

protected:
	class USoundBase* UseSound;

private:
	class UNiagaraSystem* HitEffect;
};
