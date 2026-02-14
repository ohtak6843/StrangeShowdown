// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/STPlayerBase.h"
#include "STLocalPlayer.generated.h"

UENUM(BlueprintType)
enum class ECameraPose : uint8
{
	Idle		UMETA(DisplayName = "Idle"),
	Aiming		UMETA(DisplayName = "Aiming"),
	LookingUp	UMETA(DisplayName = "LookingUp")
};

USTRUCT(BlueprintType)
struct FCameraPoseSetting
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpringArmLength = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CameraY = 0.f;
};

UCLASS()
class STRANGESHOWDOWN_API ASTLocalPlayer : public ASTPlayerBase
{
	GENERATED_BODY()
	
public:
	// Sets default values for this character's properties
	ASTLocalPlayer();

	void SetCameraPose(ECameraPose NewPose);

	UFUNCTION(BlueprintCallable)
	void ApplyStateSettings(ECameraPose NewState);

	UFUNCTION(BlueprintCallable)
	void Interact(int32& OutAddedInventoryIndex);

	// 아이템 장착 관련
	UFUNCTION(BlueprintCallable, Category = "Item")
	void HoldItem();

	UFUNCTION(BlueprintCallable, Category = "Item")
	void DropItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

public:
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
};
