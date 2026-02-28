// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/STSheriffAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/Sheriff/STSheriffBase.h"

USTSheriffAnimInstance::USTSheriffAnimInstance()
{
}

void USTSheriffAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
}

void USTSheriffAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (Movement)
	{
		Direction = CalculateDirection(Velocity, Owner->GetActorRotation());
	}

	ASTSheriffBase* Sheriff = Cast<ASTSheriffBase>(Owner);
	if (Sheriff)
	{
		bIsAiming = Sheriff->HasAnyState(ESheriffState::Aiming);
		Pitch = Sheriff->GetBaseAimRotation().Pitch;
	}
}
