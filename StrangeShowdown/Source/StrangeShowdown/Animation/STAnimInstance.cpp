// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/STAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

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
		GroundSpeed = Velocity.Size2D();
		bIsIdle = GroundSpeed < MovingThreshold;
		bIsFalling = Movement->IsFalling();
		bIsJumping = bIsFalling & (Velocity.Z > JumpingThreshold);
	}
}

void USTAnimInstance::SetAnimationValue(const float GroundSpeed_, const float Pitch_, const float Direction_)
{
	GroundSpeed = GroundSpeed_;
	// TODO: 가상함수 할지, 아니면 오버로딩 할지 고민
	//Pitch = Pitch_;
	//Direction = Direction_;
}
