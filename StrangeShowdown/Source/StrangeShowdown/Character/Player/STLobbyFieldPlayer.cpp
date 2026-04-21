// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/STLobbyFieldPlayer.h"
#include "Animation/STAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

FOnFieldPlayerSpawned ASTLobbyFieldPlayer::OnFieldPlayerSpawned;
FOnFieldPlayerRemoved ASTLobbyFieldPlayer::OnFieldPlayerRemoved;

ASTLobbyFieldPlayer::ASTLobbyFieldPlayer()
{
	// Stat Widget Component
	StatWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("StatWidgetComponent"));
	StatWidgetComponent->SetupAttachment(RootComponent);
	StatWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
}

void ASTLobbyFieldPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (StatWidgetComponent && CachedCameraManager)
	{
		FVector CameraLocation;
		FRotator CameraRotation;

		CachedCameraManager->GetCameraViewPoint(CameraLocation, CameraRotation);

		FVector Direction = CameraLocation - StatWidgetComponent->GetComponentLocation();
		FRotator LookAtRotation = FRotationMatrix::MakeFromX(Direction).Rotator();
		StatWidgetComponent->SetWorldRotation(LookAtRotation);
	}

#if NETWORK_ENABLED

	FVector CurrentLocation{ GetActorLocation() };
	FVector NewLocation{ FMath::VInterpTo(CurrentLocation, TargetLocation, DeltaTime, MoveSpeed) };
	SetActorLocation(NewLocation);

	FRotator CurrentRotation{ GetActorRotation() };
	FRotator NewRotation{ FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, RotationSpeed) };
	SetActorRotation(NewRotation);

#endif
}

void ASTLobbyFieldPlayer::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC)
	{
		CachedCameraManager = PC->PlayerCameraManager;
	}

	// TODO: 서버에서 받은 ID와 NickName으로
	static uint64 TempID = 1; // 임시 ID
	PlayerID = TempID++;
	OnFieldPlayerSpawned.Broadcast(PlayerID, FString("Temp" + FString::FromInt(PlayerID)), false);
}

void ASTLobbyFieldPlayer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	OnFieldPlayerRemoved.Broadcast(PlayerID);
}

void ASTLobbyFieldPlayer::Move(const FVector& Location, const FRotator& Rotator)
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
