// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/STCharacter.h"
#include "Types/PlayerTypes.h"
#include "STSheriffBase.generated.h"

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API ASTSheriffBase : public ASTCharacter
{
	GENERATED_BODY()

public:
	ASTSheriffBase();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State", meta = (Bitmask, BitmaskEnum = "ESheriffState"))
	uint8 SheriffState;

private:
	
};
