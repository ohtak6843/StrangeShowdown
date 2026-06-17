// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/STFieldPlayer.h"
#include "Animation/STAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UI/STWidgetComponent.h"

#include "GameFramework/GameModeBase.h"
#include "Controller/STGhostController.h"
#include "Character/Ghost/STLocalGhost.h"

ASTFieldPlayer::ASTFieldPlayer()
{
	// Stat Widget Component
	StatWidgetComponent = CreateDefaultSubobject<USTWidgetComponent>(TEXT("StatWidgetComponent"));
	StatWidgetComponent->SetupAttachment(RootComponent);
	StatWidgetComponent->SetWidgetSpace(EWidgetSpace::World);

	// Target Indicator Component
	TargetIndicatorComponent = CreateDefaultSubobject<USTTargetIndicatorComponent>(TEXT("TargetIndicatorComponent"));
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

	TickMove(DeltaTime);

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

void ASTFieldPlayer::ChangeToGhost()
{
	FTransform SpawnTransform = this->GetActorTransform();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ASTLocalGhost* NewGhost = GetWorld()->SpawnActor<ASTLocalGhost>(GhostClass, SpawnTransform, SpawnParams);
	if (NewGhost)
	{
		this->Destroy();
	}
}

void ASTFieldPlayer::Init(const FPlayerInfo& PlayerInfo)
{
	ID = PlayerInfo.ID;

	StatComp->InitPlayerStats();
}
