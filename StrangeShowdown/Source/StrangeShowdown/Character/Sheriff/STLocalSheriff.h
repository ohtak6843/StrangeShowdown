// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Sheriff/STSheriffBase.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
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

protected:
	// Spring Arm Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<USpringArmComponent> SpringArmComp;

	// Camera Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> CameraComp;

private:
	
	
};
