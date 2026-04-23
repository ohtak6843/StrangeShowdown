// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/STFieldPlayer.h"
#include "Animation/STAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

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

void ASTFieldPlayer::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC)
	{
		CachedCameraManager = PC->PlayerCameraManager;
	}
}

