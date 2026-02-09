// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/STAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/STPlayerBase.h"

USTAnimInstance::USTAnimInstance()
{
	MovingThreshold = 3.0f;
	JumpingThreshold = 100.0f;
}

void USTAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Owner = Cast<ACharacter>(GetOwningActor());
	if (Owner)
	{
		Movement = Owner->GetCharacterMovement();
	}
}

void USTAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (Movement)
	{
		Velocity = Movement->Velocity;
		Direction = CalculateDirection(Velocity, Owner->GetActorRotation());
		GroundSpeed = Velocity.Size2D();
		bIsIdle = GroundSpeed < MovingThreshold;
		bIsFalling = Movement->IsFalling();
		bIsJumping = bIsFalling & (Velocity.Z > JumpingThreshold);

	}

	TObjectPtr<ASTPlayerBase> Player = Cast<ASTPlayerBase>(Owner);
	if (Player)
	{
		bIsArmedRifle = Player->bIsArmedRifle;
		bIsAiming = Player->bIsAiming;
		bIsLookingUp = Player->bIsLookingUp;
		Pitch = Player->GetBaseAimRotation().Pitch;
	}
}

void USTAnimInstance::SetAnimationValue(const float GroundSpeed_, const float Pitch_, const float Direction_)
{
	GroundSpeed = GroundSpeed_;
	Pitch = Pitch_;
	Direction = Direction_;
}
