// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Player/STPlayerBase.h"
#include "Types/PlayerTypes.h"
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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

public:
	// ISTAnimAttackInterface
	virtual void AttackHitCheck() override;

	void SetCameraPose(ECameraPose NewPose);

	UFUNCTION(BlueprintImplementableEvent)
	void OnStatUIUpdated();

	UFUNCTION(BlueprintCallable)
	void ApplyStateSettings(ECameraPose NewState);

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
	void UseItemEffect(struct FInventorySlot slot, EItemUseType UseType);

	UFUNCTION(BlueprintImplementableEvent)
	void NotEnoughStaminaCostFloatingMessage();

	// TODO: 임시로 블프에 함수 만들어놓은 것, 나중에 함수들 c++로 옮기면서 없애야 함
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateQuickslotForCpp();

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

// HUD Section
protected:
	virtual void SetupHUDWidget(class USTHUDWidget* InHUDWidget) override;

// Camera Pose Section
private:
	void ChangeToIdle();
	void ChangeToAiming();
	void ChangeToLookingUp();

	void SendMovePacket(const float DeltaTime);

	TMap<ECameraPose, FCameraPoseSetting> PoseSettings;

	float PoseBlendTime = 0.2f;
	float PoseElapsedTime = 0.f;

	FCameraPoseSetting StartPose;
	FCameraPoseSetting TargetPose;

	// Network Section
private:
	float SendMoveDeltaTime{};
	const float SendMoveMaxTime{ 0.1f };

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TObjectPtr<class UNiagaraSystem> HitEffect;
};
