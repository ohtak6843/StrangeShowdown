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

	// Player Meshes
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> BadguyMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/PolygonWestern/Meshes/CharactersUE4Mannequin/SK_Chr_Badguy_01.SK_Chr_Badguy_01'"));
	ensure(BadguyMesh.Object);
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> BusinessManMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/PolygonWestern/Meshes/CharactersUE4Mannequin/SK_Chr_Business_Man_01.SK_Chr_Business_Man_01'"));
	ensure(BusinessManMesh.Object);
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CowboyMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/PolygonWestern/Meshes/CharactersUE4Mannequin/SK_Chr_Cowboy_01.SK_Chr_Cowboy_01'"));
	ensure(CowboyMesh.Object);
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CowgirlMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/PolygonWestern/Meshes/CharactersUE4Mannequin/SK_Chr_Cowgirl_01.SK_Chr_Cowgirl_01'"));
	ensure(CowgirlMesh.Object);
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> GunmanMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/PolygonWestern/Meshes/CharactersUE4Mannequin/SK_Chr_Gunman_01.SK_Chr_Gunman_01'"));
	ensure(GunmanMesh.Object);
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> WomanMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/PolygonWestern/Meshes/CharactersUE4Mannequin/SK_Chr_Woman_01.SK_Chr_Woman_01'"));
	ensure(WomanMesh.Object);
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> WorkingGirlMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/PolygonWestern/Meshes/CharactersUE4Mannequin/SK_Chr_WorkingGirl_01.SK_Chr_WorkingGirl_01'"));
	ensure(WorkingGirlMesh.Object);
	PlayerMeshes.Add(EPlayerMeshType::Badguy, BadguyMesh.Object);
	PlayerMeshes.Add(EPlayerMeshType::BusinessMan, BusinessManMesh.Object);
	PlayerMeshes.Add(EPlayerMeshType::Cowboy, CowboyMesh.Object);
	PlayerMeshes.Add(EPlayerMeshType::Cowgirl, CowgirlMesh.Object);
	PlayerMeshes.Add(EPlayerMeshType::Gunman, GunmanMesh.Object);
	PlayerMeshes.Add(EPlayerMeshType::Woman, WomanMesh.Object);
	PlayerMeshes.Add(EPlayerMeshType::WorkingGirl, WorkingGirlMesh.Object);

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

	// Set Local Player Mesh
	if (IsValid(PlayerMeshes[PlayerMeshType]))
	{
		USkeletalMesh* PlayerMesh = PlayerMeshes[PlayerMeshType];
		GetMesh()->SetSkeletalMesh(PlayerMesh);
	}

	// Set Dynamic Material
	DynamicMaterial = UMaterialInstanceDynamic::Create(GetMesh()->GetMaterial(0), this);
	GetMesh()->SetMaterial(0, DynamicMaterial);
}

void ASTPlayerBase::SetMaxWalkSpeed(float NewMaxWalkSpeed)
{
	GetCharacterMovement()->MaxWalkSpeed = NewMaxWalkSpeed;
}

void ASTPlayerBase::HandleStatusUpdate(const Common::SCStatusUpdate& Packet)
{
	StatComp->HandleStatusUpdate(Packet);
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

void ASTPlayerBase::SetDead()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	PlayDeadAnimation();
	SetActorEnableCollision(false);
}

void ASTPlayerBase::PlayDeadAnimation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->StopAllMontages(0.0f);
	if (IsValid(DeadMontage))
	{
		AnimInstance->Montage_Play(DeadMontage);
	}

	FTimerHandle DeadTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda(
		[&]()
		{
			PlayDissolveEffect();
			FTimerHandle DestroyTimerHandle;
			GetWorld()->GetTimerManager().SetTimer(DestroyTimerHandle, FTimerDelegate::CreateLambda(
				[&]()
				{
					GetMesh()->SetVisibility(false);
					RightHandStaticMesh->SetVisibility(false);
					RightHandSkeletalMesh->SetVisibility(false);
				}
			), DissolveEffectDelayTime, false);
		}), DeadEventDelayTime, false);
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