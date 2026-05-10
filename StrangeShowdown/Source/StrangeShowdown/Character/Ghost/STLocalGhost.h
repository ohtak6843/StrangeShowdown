// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Ghost/STGhostBase.h"
#include "InputActionValue.h"
#include "Interface/STCharacterHUDInterface.h"
#include "STLocalGhost.generated.h"

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API ASTLocalGhost : public ASTGhostBase, public ISTCharacterHUDInterface
{
	GENERATED_BODY()

public:
	ASTLocalGhost();

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay() override;

// HUD Section
protected:
	virtual void SetupHUDWidget(class USTHUDWidget* InHUDWidget) override;

// Component Section
protected:
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

// Input Section
protected:
	void ShoulderMove(const FInputActionValue& Value);
	void ShoulderLook(const FInputActionValue& Value);
	void PossessSheriff(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ShoulderMoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ShoulderLookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> PossessSheriffAction;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Possess)
	TSubclassOf<class ASTSheriffController> SheriffControllerClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Possess)
	TSubclassOf<class ASTLocalSheriff> SheriffClass;
};
