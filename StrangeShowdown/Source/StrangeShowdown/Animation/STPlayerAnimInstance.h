// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/STAnimInstance.h"
#include "STPlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API USTPlayerAnimInstance : public USTAnimInstance
{
	GENERATED_BODY()

public:
	USTPlayerAnimInstance();

protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	float Direction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	uint8 bIsAiming : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	uint8 bIsLookingUp : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	float Pitch;

private:
	
};
