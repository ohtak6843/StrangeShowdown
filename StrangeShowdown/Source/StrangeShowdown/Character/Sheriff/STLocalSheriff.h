// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Sheriff/STSheriffBase.h"
//#include "InputActionValue.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<class UInputMappingContext> DefaultMappingContext;

private:
	
	
};
