// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actor/MapObject/STMapObjectBase.h"
#include "STBankFirstMapObject.generated.h"

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API ASTBankFirstMapObject : public ASTMapObjectBase
{
	GENERATED_BODY()
	
public:
	virtual void ActivationMapObject(APawn* Interactor) override;
};
