// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/STCharacter.h"
#include "STSheriffBase.generated.h"

UENUM(meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class ESheriffState : uint8
{
	None			= 0				UMETA(DisplayName = "None"),
	Idle			= 1 << 0		UMETA(DisplayName = "Idle"),
	Jumping			= 1 << 1		UMETA(DisplayName = "Jumping"),
	Dead			= 1 << 2		UMETA(DisplayName = "Dead"),
};
ENUM_CLASS_FLAGS(ESheriffState)

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
