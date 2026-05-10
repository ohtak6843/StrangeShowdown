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

}

void ASTLobbyFieldPlayer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	OnFieldPlayerRemoved.Broadcast(PlayerID);
}

void ASTLobbyFieldPlayer::Init(const FPlayerInfo& PlayerInfo)
{
	PlayerID = PlayerInfo.PlayerID;
	OnFieldPlayerSpawned.Broadcast(PlayerID, PlayerInfo.NickName, false);
	UE_LOG(LogTemp, Log, TEXT("Lobby Field Player Initialized: PlayerID=%llu"), PlayerID);
}
