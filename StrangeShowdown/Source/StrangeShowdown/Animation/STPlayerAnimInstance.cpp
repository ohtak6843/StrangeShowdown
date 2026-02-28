// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/STPlayerAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/Player/STPlayerBase.h"

USTPlayerAnimInstance::USTPlayerAnimInstance()
{
}

void USTPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
}

void USTPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (Movement)
	{
		Direction = CalculateDirection(Velocity, Owner->GetActorRotation());
	}

	ASTPlayerBase* Player = Cast<ASTPlayerBase>(Owner);
	if (Player)
	{
		bIsAiming = Player->HasAnyState(EPlayerState::Aiming);
		bIsLookingUp = Player->HasAnyState(EPlayerState::LookingUp);
		Pitch = Player->GetBaseAimRotation().Pitch;
	}
}
