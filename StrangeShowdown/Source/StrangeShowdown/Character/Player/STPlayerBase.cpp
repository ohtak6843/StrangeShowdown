// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/STPlayerBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/STAnimInstance.h"
#include "CommonDefine.h"

#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

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
	FSTCharacterStat InitialStat;
	InitialStat.CurrentHp = InitialStat.MaxHp;
	InitialStat.CurrentGold = 0;
	InitialStat.KillCount = 0;
	InitialStat.CurrentArmor = 0;
	InitialStat.CurrentStamina = InitialStat.MaxStamina - 2;
	InitialStat.CurrentAction = InitialStat.UsableAction;
	InitialStat.Bounty = 0;

	// Right Hand Mesh
	RightHandStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightItemMesh"));
	RightHandStaticMesh->SetupAttachment(GetMesh());

	RightHandSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RightWeaponMesh"));
	RightHandSkeletalMesh->SetupAttachment(GetMesh());

	// Item Use Effect
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> ParticleEffectRef(TEXT("/Script/Niagara.NiagaraSystem'/Game/StrangeShowdown/Item/FX/NS_UseEffect.NS_UseEffect'"));
	if (ParticleEffectRef.Object)
	{
		ItemUseEffect = ParticleEffectRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> HelmetUseSound(TEXT("/Script/Engine.SoundWave'/Game/StrangeShowdown/Sound/Effect/SW_UseHelmet.SW_UseHelmet'"));
	static ConstructorHelpers::FObjectFinder<USoundBase> MeatUseSound(TEXT("/Script/Engine.SoundWave'/Game/StrangeShowdown/Sound/Effect/SW_EatMeat.SW_EatMeat'"));
	static ConstructorHelpers::FObjectFinder<USoundBase> WhiskeyUseSound(TEXT("/Script/Engine.SoundWave'/Game/StrangeShowdown/Sound/Effect/SW_DrinkWhiskey.SW_DrinkWhiskey'"));
	static ConstructorHelpers::FObjectFinder<USoundBase> EnhancePowerUseSound(TEXT("/Script/Engine.SoundWave'/Game/StrangeShowdown/Sound/Effect/SW_EnhancePower.SW_EnhancePower'"));
	static ConstructorHelpers::FObjectFinder<USoundBase> LetterUseSound(TEXT("/Script/Engine.SoundWave'/Game/StrangeShowdown/Sound/Effect/SW_Mission.SW_Mission'"));
	static ConstructorHelpers::FObjectFinder<USoundBase> WheelUseSound(TEXT("/Script/Engine.SoundWave'/Game/StrangeShowdown/Sound/Effect/SW_Wheel.SW_Wheel'"));

	ItemUseSounds.Add(EItemType::Helmet, HelmetUseSound.Object);
	ItemUseSounds.Add(EItemType::Meat, MeatUseSound.Object);
	ItemUseSounds.Add(EItemType::Whiskey, WhiskeyUseSound.Object);
	ItemUseSounds.Add(EItemType::EnhancePower, EnhancePowerUseSound.Object);
	ItemUseSounds.Add(EItemType::Letter, LetterUseSound.Object);
	ItemUseSounds.Add(EItemType::Wheel, WheelUseSound.Object);
}

void ASTPlayerBase::BeginPlay()
{
	Super::BeginPlay();
}

void ASTPlayerBase::SetMaxWalkSpeed(float NewMaxWalkSpeed)
{
	GetCharacterMovement()->MaxWalkSpeed = NewMaxWalkSpeed;
}

float ASTPlayerBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (ActualDamage <= 0.f)
	{
		return 0.f;
	}

	FSTCharacterStat& CharacterStat = StatComp->GetCharacterStat();
	StatComp->SetCurrentHp(CharacterStat.CurrentHp - ActualDamage);

	// TODO: Check Death
	if (CharacterStat.CurrentHp <= 0.f)
	{

	}

	return ActualDamage;
}

void ASTPlayerBase::PlayItemUseEffect(EItemType ItemType)
{
	if (EItemType::Helmet > ItemType) return;

	if(IsValid(ItemUseEffect))
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ItemUseEffect, GetActorLocation());
	}

	if (IsValid(ItemUseSounds[ItemType]))
	{
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), ItemUseSounds[ItemType], GetActorLocation());
	}
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
		// STAnimInst->SetAnimationValue(Speed, Rotator.Pitch, Rotator.Yaw);

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

