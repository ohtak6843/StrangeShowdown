// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Sheriff/STSheriffBase.h"
#include "InputActionValue.h"
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

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

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

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MiniMap")
	TObjectPtr<class ASTMiniMapActor> MiniMapActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "BigMap")
	TObjectPtr<class ASTBigMapActor> BigMapActor;

private:
	void InputMappingContextAdd();
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

};
