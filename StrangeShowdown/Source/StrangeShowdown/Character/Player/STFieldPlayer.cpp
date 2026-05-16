// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/STFieldPlayer.h"
#include "Animation/STAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UI/STWidgetComponent.h"

ASTFieldPlayer::ASTFieldPlayer()
{
	// Stat Widget Component
	StatWidgetComponent = CreateDefaultSubobject<USTWidgetComponent>(TEXT("StatWidgetComponent"));
	StatWidgetComponent->SetupAttachment(RootComponent);
	StatWidgetComponent->SetWidgetSpace(EWidgetSpace::World);

	// Target Indicator Component
	TargetIndicatorComponent = CreateDefaultSubobject<USTTargetIndicatorComponent>(TEXT("TargetIndicatorComponent"));

	UE_LOG(LogTemp, Warning, TEXT("ASTFieldPlayer Constructor called"));
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

void ASTFieldPlayer::Init(const FPlayerInfo& PlayerInfo)
{
	PlayerID = PlayerInfo.PlayerID;

	StatComp->InitPlayerStats();
}
