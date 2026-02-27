// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/STAnimInstance.h"
#include "STSheriffAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API USTSheriffAnimInstance : public USTAnimInstance
{
	GENERATED_BODY()

public:
	USTSheriffAnimInstance();

protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	
};
