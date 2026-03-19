// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Player/STPlayerBase.h"
#include "Types/PlayerTypes.h"
#include "Interface/STAnimAttackInterface.h"
#include "STLocalPlayer.generated.h"

UCLASS()
class STRANGESHOWDOWN_API ASTLocalPlayer : public ASTPlayerBase, public ISTAnimAttackInterface
{
	GENERATED_BODY()
	
public:
	// Sets default values for this character's properties
	ASTLocalPlayer();

	virtual void AttackHitCheck() override;

	void SetCameraPose(ECameraPose NewPose);

	UFUNCTION(BlueprintImplementableEvent)
	void OnStatUIUpdated();

	UFUNCTION(BlueprintCallable)
	void ApplyStateSettings(ECameraPose NewState);

	UFUNCTION(BlueprintCallable)
	void Interact(int32& OutAddedInventoryIndex);

	// 아이템 장착 관련
	UFUNCTION(BlueprintCallable, Category = "Item")
	void HoldItem();

	UFUNCTION(BlueprintCallable, Category = "Item")
	void DropItem();

	// TODO: 임시로 블프에 함수 만들어놓은 것, 나중에 함수들 c++로 옮기면서 없애야 함
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateQuickslotForCpp();

	TObjectPtr<class UCameraComponent> GetCameraComp() { return CameraComp; }

	TObjectPtr<class USTStoreComponent> GetStoreComp() { return StoreComp; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Smash")
	bool IsSmashing = false;

private:
	// Change Camera Settings with State
	void ChangeToIdle();
	void ChangeToAiming();
	void ChangeToLookingUp();

	void SendMovePacket(const float DeltaTime);

	TMap<ECameraPose, FCameraPoseSetting> PoseSettings;

	float PoseBlendTime = 0.2f;
	float PoseElapsedTime = 0.f;

	FCameraPoseSetting StartPose;
	FCameraPoseSetting TargetPose;

	// Network
	float SendMoveDeltaTime{};
	const float SendMoveMaxTime{ 0.1f };

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TObjectPtr<class UNiagaraSystem> HitEffect;
};
