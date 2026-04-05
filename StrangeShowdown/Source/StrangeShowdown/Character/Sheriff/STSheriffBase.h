// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/STCharacter.h"
#include "GameData/STTypes.h"
#include "Component/STStatComponent.h"
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

// State Section
public:
	FORCEINLINE void AddState(ESheriffState NewState) { SheriffStateFlag |= static_cast<uint8>(NewState); }
	FORCEINLINE void RemoveState(ESheriffState RemoveState) { SheriffStateFlag &= ~static_cast<uint8>(RemoveState); }
	FORCEINLINE bool HasAnyState(ESheriffState CheckState) const { return (SheriffStateFlag & static_cast<uint8>(CheckState)) != 0; }
	FORCEINLINE bool HasAllState(ESheriffState CheckState) const { return (SheriffStateFlag & static_cast<uint8>(CheckState)) == static_cast<uint8>(CheckState); }
	FORCEINLINE bool SetState(ESheriffState NewState) { SheriffStateFlag = static_cast<uint8>(NewState); return true; }

	UFUNCTION(BlueprintPure)
	bool HasAnyStateMask(UPARAM(meta = (Bitmask, BitmaskEnum = "ESheriffState")) int32 CheckState) const { return HasAnyState(static_cast<ESheriffState>(CheckState)); }

	UFUNCTION(BlueprintPure)
	bool HasAllStateMask(UPARAM(meta = (Bitmask, BitmaskEnum = "ESheriffState")) int32 CheckState) const { return HasAllState(static_cast<ESheriffState>(CheckState)); }

	UFUNCTION(BlueprintPure)
	bool SetStateMask(UPARAM(meta = (Bitmask, BitmaskEnum = "ESheriffState")) int32 NewState) { return SetState(static_cast<ESheriffState>(NewState)); }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State, meta = (Bitmask, BitmaskEnum = "ESheriffState"))
	uint8 SheriffStateFlag;

// Mesh
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class USkeletalMeshComponent> PistolMesh;

// Stat Component
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stats)
	TObjectPtr<USTStatComponent> StatComp;
	
};
