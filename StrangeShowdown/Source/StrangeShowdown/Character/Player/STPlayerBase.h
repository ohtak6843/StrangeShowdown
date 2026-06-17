// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/STCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Component/STStatComponent.h"
#include "GameData/STTypes.h"
#include "STPlayerBase.generated.h"

UCLASS()
class STRANGESHOWDOWN_API ASTPlayerBase : public ASTCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASTPlayerBase();

protected:
	virtual void BeginPlay() override;

public:
	FORCEINLINE void AddState(EPlayerState NewState) { PlayerStateFlag |= static_cast<uint8>(NewState); }
	FORCEINLINE void RemoveState(EPlayerState RemoveState) { PlayerStateFlag &= ~static_cast<uint8>(RemoveState); }
	FORCEINLINE bool HasAnyState(EPlayerState CheckState) const { return (PlayerStateFlag & static_cast<uint8>(CheckState)) != 0; }
	FORCEINLINE bool HasAllState(EPlayerState CheckState) const { return (PlayerStateFlag & static_cast<uint8>(CheckState)) == static_cast<uint8>(CheckState); }
	FORCEINLINE bool SetState(EPlayerState NewState) { PlayerStateFlag = static_cast<uint8>(NewState); return true; }

	UFUNCTION(BlueprintPure)
	bool HasAnyStateMask(UPARAM(meta = (Bitmask, BitmaskEnum = "EPlayerState")) int32 CheckState) const { return HasAnyState(static_cast<EPlayerState>(CheckState)); }

	UFUNCTION(BlueprintPure)
	bool HasAllStateMask(UPARAM(meta = (Bitmask, BitmaskEnum = "EPlayerState")) int32 CheckState) const { return HasAllState(static_cast<EPlayerState>(CheckState)); }

	UFUNCTION(BlueprintPure)
	bool SetStateMask(UPARAM(meta = (Bitmask, BitmaskEnum = "EPlayerState")) int32 NewState) { return SetState(static_cast<EPlayerState>(NewState)); }

	void SetMaxWalkSpeed(float NewMaxWalkSpeed);

public:
	// Stat Component
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stats")
	TObjectPtr<USTStatComponent> StatComp;

	// Player Mesh Type
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	EPlayerMeshType PlayerMeshType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State", meta = (Bitmask, BitmaskEnum = "EPlayerState"))
	uint8 PlayerStateFlag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TObjectPtr<class USTItemDataAssetBase> HoldingItemData;

protected:
	// 오른손 아이템 메쉬
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UStaticMeshComponent> RightHandStaticMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USkeletalMeshComponent> RightHandSkeletalMesh;

// Player Meshes Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TSoftObjectPtr<class USkeletalMesh>> PlayerMeshes;

// Damage Section
public:
	UFUNCTION(BlueprintCallable, Category = "Stats")
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

// Dead Section
public:
	UFUNCTION(BlueprintCallable)
	virtual void SetDead();
	void PlayDeadAnimation();

	UFUNCTION(BlueprintNativeEvent)
	void PlayDissolveEffect();
	virtual void PlayDissolveEffect_Implementation() {}

	UFUNCTION(BlueprintCallable)
	virtual void ChangeToGhost() {}

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Dead)
	TObjectPtr<class UMaterialInstanceDynamic> DynamicMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Dead)
	TObjectPtr<class UAnimMontage> DeadMontage;

	float DeadEventDelayTime = 3.0f;
	float DissolveEffectDelayTime = 2.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class ASTGhostController> GhostControllerClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class ASTGhostBase> GhostClass;

// Effect Section
public:
	UFUNCTION(BlueprintCallable, Category = Effect)
	void PlayItemUseEffect(EItemType ItemType);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Effect)
	TObjectPtr<class UNiagaraSystem> ItemUseEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Effect)
	TMap<EItemType, TObjectPtr<class USoundBase>> ItemUseSounds;

// Network Section
public:
	void Move(const FVector& Location, const FRotator& Rotator);

protected:
	FVector TargetLocation{};
	FRotator TargetRotation{};

};
