// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/STPlayerBase.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ASTPlayerBase::ASTPlayerBase()
{
	// Pawn
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Character Movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Stat Component
	StatComp = CreateDefaultSubobject<USTStatComponent>(TEXT("StatComp"));
	StatComp->Hp = StatComp->MaxHp;
	StatComp->Gold = 0;
	StatComp->Kill = 0;
	StatComp->Armor = 0;
	StatComp->MoveSpeed = 500;
	StatComp->Stamina = StatComp->MaxStamina - 2;
	StatComp->Action = StatComp->MaxAction;
	StatComp->Prize = 0;
	StatComp->bAlive = true;
}

void ASTPlayerBase::BeginPlay()
{
	Super::BeginPlay();
}

