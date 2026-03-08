// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/NPC/STStorekeeper.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Prop/STCarriage.h"

ASTStorekeeper::ASTStorekeeper()
{
	// Pawn
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);
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
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -104.f), FRotator(0.f, -90.f, 0.f));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	GetMesh()->SetCollisionProfileName(TEXT("CharacterMesh"));

	// Set Skeletal Mesh
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/PolygonWestern/Meshes/CharactersUE4Mannequin/SK_Chr_Business_Man_01.SK_Chr_Business_Man_01'"));
	if (CharacterMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(CharacterMeshRef.Object);
	}

	// Set Single Animation
	static ConstructorHelpers::FObjectFinder<UAnimationAsset> AnimationRef(TEXT("/Script/Engine.BlendSpace'/Game/PolygonWestern/EpicContent/Mannequin/RetargetAnimations/Unarmed/BS_UnarmedIdleWalkRun.BS_UnarmedIdleWalkRun'"));
	if (AnimationRef.Object)
	{
		GetMesh()->AnimationData.AnimToPlay = AnimationRef.Object;
		GetMesh()->AnimationData.bSavedLooping = true;
		GetMesh()->AnimationData.bSavedPlaying = true;
	}

	// Carriage
	Carriage = CreateDefaultSubobject<UChildActorComponent>(TEXT("Carriage"));
	Carriage->SetChildActorClass(ASTCarriage::StaticClass());
	Carriage->SetupAttachment(GetMesh());
	Carriage->SetRelativeLocationAndRotation(FVector(0.f, -120.f, 118.f), FRotator(0.f, 0.f, 0.f));
	Carriage->SetRelativeScale3D(FVector(2.f, 2.f, 2.f));
}


void ASTStorekeeper::BeginPlay()
{
	Super::BeginPlay();
}
