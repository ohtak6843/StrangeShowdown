// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/STPlayerBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/STAnimInstance.h"
#include "CommonDefine.h"

// Sets default values
ASTPlayerBase::ASTPlayerBase()
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
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetCollisionProfileName(TEXT("ChracterMesh"));

	// Stat Component
	StatComp = CreateDefaultSubobject<USTStatComponent>(TEXT("StatComp"));

	// Right Hand Mesh
	RightHandStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightItemMesh"));
	RightHandStaticMesh->SetupAttachment(GetMesh());

	RightHandSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RightWeaponMesh"));
	RightHandSkeletalMesh->SetupAttachment(GetMesh());
}


float ASTPlayerBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (ActualDamage <= 0.f)
	{
		return 0.f;
	}

	StatComp->AddHp(-ActualDamage);

	// TODO: Check Death
	if (StatComp->CurrentHp <= 0.f)
	{
		
	}

	return ActualDamage;
}

void ASTPlayerBase::BeginPlay()
{
	Super::BeginPlay();
}

void ASTPlayerBase::Move(const FVector& Location, const FRotator& Rotator)
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

