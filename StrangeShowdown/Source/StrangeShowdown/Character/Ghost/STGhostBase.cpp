// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ghost/STGhostBase.h"
#include "Components/CapsuleComponent.h"
#include "Component/STStatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ASTGhostBase::ASTGhostBase()
{
	// Pawn
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 84.f);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));

	// Movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Mesh
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, 0.f), FRotator(0.f, -90.f, 0.f));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetCollisionProfileName(TEXT("ChracterMesh"));

	// Static Mesh
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GhostMesh"));
	StaticMesh->SetupAttachment(GetMesh());
	StaticMesh->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
	StaticMesh->SetRelativeLocationAndRotation(FVector(0.f, 0.f, 30.f), FRotator(0.f, 0.f, 0.f));
	StaticMesh->SetCollisionProfileName(TEXT("ChracterMesh"));

	// Set Static Mesh
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CharacterMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/StrangeShowdown/Character/Ghost/StaticMeshes/Ghost.Ghost'"));
	if (CharacterMeshRef.Object)
	{
		StaticMesh->SetStaticMesh(CharacterMeshRef.Object);
	}

	// Stat Component
	StatComp = CreateDefaultSubobject<USTStatComponent>(TEXT("StatComp"));
}
