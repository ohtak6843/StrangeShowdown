// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/STCharacter.h"
#include "STStorekeeper.generated.h"

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API ASTStorekeeper : public ASTCharacter
{
	GENERATED_BODY()

public:
	ASTStorekeeper();

protected:
	virtual void BeginPlay() override;

private:
};
