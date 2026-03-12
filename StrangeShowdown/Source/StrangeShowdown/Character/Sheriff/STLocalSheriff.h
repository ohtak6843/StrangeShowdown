// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Sheriff/STSheriffBase.h"
#include "InputActionValue.h"
#include "Types/PlayerTypes.h"
#include "STLocalSheriff.generated.h"

/**
 * 
 */

UCLASS()
class STRANGESHOWDOWN_API ASTLocalSheriff : public ASTSheriffBase
{
	GENERATED_BODY()

public:
	ASTLocalSheriff();

	void SetCameraPose(ECameraPose NewPose);

	UFUNCTION(BlueprintCallable)
	void ApplyStateSettings(ECameraPose NewState);

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 블루프린트에서 호출할 Fire()
	UFUNCTION(BlueprintImplementableEvent)
	void Fire_BP();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	// Spring Arm Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<class USpringArmComponent> SpringArmComp;

	// Camera Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<class UCameraComponent> CameraComp;

	// Input Mapping Context
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<class UInputMappingContext> DefaultMappingContext;

	// QuickSlot Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "QuickSlot")
	TObjectPtr<class USTQuickSlotComponent> QuickSlotComp;

	// Attack Trace Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AttackTrace")
	TObjectPtr<class USTAttackTraceComponent> AttackTraceComp;

private:
	void ChangeToIdle();
	void ChangeToAiming();

	void AddInputMappingContext();
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void PistolAim(const FInputActionValue& Value);
	void PistolFire(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ShoulderMoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ShoulderLookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> PistolAimAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> PistolFireAction;

	TMap<ECameraPose, FCameraPoseSetting> PoseSettings;

	float PoseBlendTime = 0.2f;
	float PoseElapsedTime = 0.f;

	FCameraPoseSetting StartPose;
	FCameraPoseSetting TargetPose;
};
