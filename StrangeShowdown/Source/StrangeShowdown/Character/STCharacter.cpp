// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/STCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/STAnimInstance.h"
#include "CommonDefine.h"
#include "Game/STGameInstance.h"


// Sets default values
ASTCharacter::ASTCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called every frame
void ASTCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASTCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

// Called when the game starts or when spawned
void ASTCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASTCharacter::Move(const FVector& Location, const FRotator& Rotator)
{
	// speed 미리 계산
	float Speed{
		static_cast<float>(FVector::Dist(Location, TargetLocation)) / Common::SendMoveTime
	};

	if (nullptr == GetMesh())
	{
		UE_LOG(LogTemp, Warning, TEXT("Mesh is not valid in Move function"));
		return;
	}

	SetActorLocation(TargetLocation);
	SetActorRotation(TargetRotation);

	// 애니메이션
	if (auto* STAnimInst{ Cast<USTAnimInstance>(GetMesh()->GetAnimInstance()) })
	{
		// STAnimInst->SetAnimationValue(Speed, Rotator.Pitch, Rotator.Yaw);

		// velocity 설정
		auto* Movement{ GetCharacterMovement() };
		if (Movement)
		{
			FVector Direction{ Location - TargetLocation };
			Movement->Velocity = Direction * Speed;
		}
	}

	TargetLocation = Location;
	TargetRotation = Rotator;


}

void ASTCharacter::TickMove(float DeltaTime)
{
	FVector CurrentLocation{ GetActorLocation() };
	FVector NewLocation{ FMath::VInterpTo(CurrentLocation, TargetLocation, DeltaTime, MoveSpeed) };
	SetActorLocation(NewLocation);

	FRotator CurrentRotation{ GetActorRotation() };
	FRotator NewRotation{ FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, RotationSpeed) };
	SetActorRotation(NewRotation);
}


void ASTCharacter::SendMovePacket(const float DeltaTime, const uint8 PlayerStateFlag)
{
	SendMoveDeltaTime += DeltaTime;

	if (SendMoveDeltaTime >= Common::SendMoveTime)
	{
		SendMoveDeltaTime -= Common::SendMoveTime;

		auto Rotation{ GetActorRotation() };
		auto Location{ GetActorLocation() };

		Cast<USTGameInstance>(GWorld->GetGameInstance())->MovePlayer(Location, Rotation, PlayerStateFlag);
	}
}

void ASTCharacter::Teleport(const FTransform& SpawnTransform)
{
	// 위치와 회전값을 적용하여 순간이동 처리 (물리 상태 및 속도 초기화 포함)
	SetActorLocationAndRotation(
		SpawnTransform.GetLocation(),
		SpawnTransform.Rotator(),
		false,
		nullptr,
		ETeleportType::TeleportPhysics
	);
}
