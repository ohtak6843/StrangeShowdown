// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Player/STPlayerBase.h"
#include "GameData/STTypes.h"
#include "InputActionValue.h"
#include "GameData/STItemSlot.h"
#include "Interface/STCharacterHUDInterface.h"
#include "Interface/STAnimAttackInterface.h"
#include "STLocalPlayer.generated.h"

UCLASS()
class STRANGESHOWDOWN_API ASTLocalPlayer : public ASTPlayerBase, public ISTAnimAttackInterface, public ISTCharacterHUDInterface
{
	GENERATED_BODY()
	
public:
	// Sets default values for this character's properties
	ASTLocalPlayer();

	virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay() override;

public:
	// ISTAnimAttackInterface
	virtual void AttackHitCheck() override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnStatUIUpdated();

	UFUNCTION(BlueprintCallable)
	void Interact(int32& OutAddedInventoryIndex);

	UFUNCTION(BlueprintCallable, Category = "Item")
	void UseItem();

	// 아이템 장착 관련
	UFUNCTION(BlueprintCallable, Category = "Item")
	void HoldItem();

	UFUNCTION(BlueprintCallable, Category = "Item")
	void DropItem();

	UFUNCTION(BlueprintImplementableEvent)
	void UseItemEffect(struct FSTItemSlot slot, EItemUseType UseType);

	UFUNCTION(BlueprintImplementableEvent)
	void NotEnoughStaminaCostFloatingMessage();

	TObjectPtr<class UCameraComponent> GetCameraComp() { return CameraComp; }

	TObjectPtr<class USTStoreComponent> GetStoreComp() { return StoreComp; }

	TObjectPtr<class USTMissionComponent> GetMissionComp() { return MissionComponent; }

protected:
	// Spring Arm Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<USpringArmComponent> SpringArmComp;

	// Camera Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> CameraComp;

	// Inventory Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<class USTInventoryComponent> InventoryComp;

	// QuickSlot Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "QuickSlot")
	TObjectPtr<class USTQuickSlotComponent> QuickSlotComp;

	// Store Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Store")
	TObjectPtr<class USTStoreComponent> StoreComp;

	// Attack Trace Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AttackTrace")
	TObjectPtr<class USTAttackTraceComponent> AttackTraceComp;

	// Mission Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mission")
	TObjectPtr<class USTMissionComponent> MissionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Smash")
	bool IsSmashing = false;

// Input Section
protected:
	void ShoulderMove(const FInputActionValue& Value);
	void ShoulderLook(const FInputActionValue& Value);
	void PistolAim(const FInputActionValue& Value);
	void PistolFire(const FInputActionValue& Value);
	void ChangeQuickSlot(const FInputActionValue& Value);
	void ScrollQuickSlot(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<class UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<class UInputAction> ShoulderMoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<class UInputAction> ShoulderLookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<class UInputAction> PistolAimAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<class UInputAction> PistolFireAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<class UInputAction> ChangeQuickSlotAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<class UInputAction> ScrollQuickSlotAction;

// Montage Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Montage)
	TObjectPtr<class UAnimMontage> PistolFireMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Montage)
	TObjectPtr<class UAnimMontage> SmashMontage;

// Effect Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Effect)
	TObjectPtr<class UNiagaraSystem> MuzzleFlashEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Effect)
	TObjectPtr<class UNiagaraSystem> HitEffect;

// Sound Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sound)
	TObjectPtr<class USoundBase> PistolFireSound;

// HUD Section
protected:
	virtual void SetupHUDWidget(class USTHUDWidget* InHUDWidget) override;

// Camera Pose Section
protected:
	void ApplyStateSettings(ECameraPose NewState);
	void SetCameraPose(ECameraPose NewPose);
	void ChangeToIdle();
	void ChangeToAiming();
	void ChangeToLookingUp();

	TMap<ECameraPose, FCameraPoseSetting> PoseSettings;

	float PoseBlendTime = 0.2f;
	float PoseElapsedTime = 0.f;

	FCameraPoseSetting StartPose;
	FCameraPoseSetting TargetPose;

// Player Meshes Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TSoftObjectPtr<class USkeletalMesh>> PlayerMeshes;

// Network Section
private:
	void SendMovePacket(const float DeltaTime);

	float SendMoveDeltaTime{};
	const float SendMoveMaxTime{ 0.1f };
};
