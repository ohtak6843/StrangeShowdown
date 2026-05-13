// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/STMineral.h"
#include "Character/Sheriff/STFieldSheriff.h"
#include "Character/Player/STPlayerBase.h"
#include "Animation/AnimSingleNodeInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Actor/STMiniMapActor.h"
#include "Actor/STBigMapActor.h"

ASTMineral::ASTMineral()
{
	SubMineralMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SubMineralMeshComponent"));
	SubMineralMeshComponent->SetupAttachment(RootComponent);

	SubMineralOutLineMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SubMineralOutLineMesh"));
	SubMineralOutLineMesh->SetupAttachment(SubMineralMeshComponent);

	SubMineralOutLineMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/StrangeShowdown/OutLine/M_OutLine1.M_OutLine1"));


	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	SkeletalMeshComponent->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UAnimSequence> IdleAnimationRef(TEXT("/Script/Engine.AnimSequence'/Game/PolygonWestern/EpicContent/Mannequin/RetargetAnimations/Unarmed/MM_Idle.MM_Idle'"));
	if (IdleAnimationRef.Object)
	{
		IdleAnimation = IdleAnimationRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UTexture2D> IconRef(
		TEXT("/Script/Engine.Texture2D'/Game/StrangeShowdown/UI/Texture/T_MineralIcon.T_MineralIcon'")
	);

	if (IconRef.Succeeded())
	{
		MiniMapIcon = IconRef.Object;
	}
}

void ASTMineral::BeginPlay()
{
	Super::BeginPlay();

	SkeletalMeshComponent->PlayAnimation(IdleAnimation, true);
	UAnimSingleNodeInstance* SingleNodeInstance = SkeletalMeshComponent->GetSingleNodeInstance();
	if (SingleNodeInstance)
	{
		SingleNodeInstance->SetPlaying(false);
	}

	// CameraManager 캐싱
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC)
	{
		CachedCameraManager = PC->PlayerCameraManager;

		ASTCharacter* LocalPlayer = Cast<ASTCharacter>(PC->GetPawn());

		if (LocalPlayer && LocalPlayer->MiniMapActor)
		{
			LocalPlayer->MiniMapActor->RegisterMiniMapTarget(this);
		}

		if (LocalPlayer && LocalPlayer->BigMapActor)
		{
			LocalPlayer->BigMapActor->RegisterBigMapTarget(this);
		}
	}
}

void ASTMineral::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (SubMineralOutLineMesh && SubMineralMeshComponent)
	{
		SubMineralOutLineMesh->SetStaticMesh(SubMineralMeshComponent->GetStaticMesh());
	}

	if (SubMineralOutLineMaterial)
	{
		SubMineralOutLineMesh->SetMaterial(0, SubMineralOutLineMaterial);
	}
}

void ASTMineral::Slice(const FVector& HitLocation, const FVector& HitNormal, ASTPlayerBase* Player)
{
	// 랜텀으로 hp 감소
	// TODO: 서버에서 랜덤값 받아오도록 수정
	int AttackHp = FMath::RandRange(1, 5);
	hp -= AttackHp;
	UE_LOG(LogTemp, Log, TEXT("Attack Hp: %d, Mineral HP: %d"), AttackHp, hp);

	// 1단계
	if (hp <= 5 && !bIsSubSliced)
	{
		Player->StatComp->AddGold(AttackHp * 2);
		OnSubSlicedBlueprint(AttackHp * 2);
		bIsSubSliced = true;
		return;
	}

	// 2단계
	if (hp <= 0 && !bIsSliced)
	{
		Super::Slice(HitLocation, HitNormal, Player);

		Player->StatComp->AddGold(AttackHp * 2);
		OnMainSlicedBlueprint(AttackHp * 2);
		SkeletalMeshComponent->SetVisibility(false);

		// FieldSheriff 스폰
		FTransform SpawnTransform(GetActorLocation());
		ASTFieldSheriff* SpawnedFieldSheriff = GetWorld()->SpawnActorDeferred<ASTFieldSheriff>(ASTFieldSheriff::StaticClass(), SpawnTransform);
		if (SpawnedFieldSheriff)
		{
			SpawnedFieldSheriff->FinishSpawning(SpawnTransform);
		}
		return;
	}

	// 부서지지 않았으면 피격 사운드 재생
	Player->StatComp->AddGold(AttackHp * 2);
	OnHitSoundBlueprint(AttackHp * 2);
}

FVector ASTMineral::GetMiniMapLocation_Implementation()
{
	return GetActorLocation();
}

UTexture2D* ASTMineral::GetMiniMapIcon_Implementation()
{
	return MiniMapIcon;
}