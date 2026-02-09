// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/STFieldPlayer.h"
#include "Animation/STAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"

ASTFieldPlayer::ASTFieldPlayer()
{
	// Stat Widget Component
	StatWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("StatWidgetComponent"));
	StatWidgetComponent->SetupAttachment(RootComponent);
	StatWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
}

void ASTFieldPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 위젯이 카메라를 바라보도록 처리
	if (StatWidgetComponent)
	{
		FVector CameraLocation;
		FRotator CameraRotation;

		// 플레이어 카메라 얻기
		APlayerController* PC = GetWorld()->GetFirstPlayerController();
		if (PC && PC->PlayerCameraManager)
		{
			PC->PlayerCameraManager->GetCameraViewPoint(CameraLocation, CameraRotation);

			// UI가 카메라를 바라보게
			FVector Direction = CameraLocation - StatWidgetComponent->GetComponentLocation();
			FRotator LookAtRotation = FRotationMatrix::MakeFromX(Direction).Rotator();
			StatWidgetComponent->SetWorldRotation(LookAtRotation);
		}
	}

	// 이동 보간
#if NETWORK_ENABLED

	FVector CurrentLocation{ GetActorLocation() };
	FVector NewLocation{ FMath::VInterpTo(CurrentLocation, TargetLocation, DeltaTime, MoveSpeed) };
	SetActorLocation(NewLocation);
	
	FRotator CurrentRotation{ GetActorRotation() };
	FRotator NewRotation{ FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, RotationSpeed) };
	SetActorRotation(NewRotation);
#endif
}

void ASTFieldPlayer::Move(const FVector& Location, const FRotator& Rotator)
{

	// speed 미리 계산
	float Speed{ 
		static_cast<float>(FVector::Dist(Location, TargetLocation)) / SendMoveMaxTime
	};

	SetActorLocation(TargetLocation);
	SetActorRotation(TargetRotation);

	// 애니메이션
	if (auto* STAnimInst{ Cast<USTAnimInstance>(GetMesh()->GetAnimInstance()) })
	{
		STAnimInst->SetAnimationValue(Speed, Rotator.Pitch, Rotator.Yaw);

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

