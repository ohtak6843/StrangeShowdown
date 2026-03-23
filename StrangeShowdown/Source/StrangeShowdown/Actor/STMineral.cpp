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

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	SkeletalMeshComponent->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UAnimSequence> IdleAnimationRef(TEXT("/Script/Engine.AnimSequence'/Game/PolygonWestern/EpicContent/Mannequin/RetargetAnimations/Unarmed/MM_Idle.MM_Idle'"));
	if (IdleAnimationRef.Object)
	{
		IdleAnimation = IdleAnimationRef.Object;
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
			LocalPlayer->MiniMapActor->RegisterMineral(this);
		}

		if (LocalPlayer && LocalPlayer->BigMapActor)
		{
			LocalPlayer->BigMapActor->RegisterMineral(this);
		}
	}
}

void ASTMineral::Slice(const FVector& HitLocation, const FVector& HitNormal, ASTPlayerBase* Player)
{
	// 1단계
	if (!bIsSubSliced)
	{
		Player->StatComp->AddGold(5);
		OnSubSlicedBlueprint();
		bIsSubSliced = true;
		return;
	}

	// 2단계
	if (!bIsSliced)
	{
		Super::Slice(HitLocation, HitNormal, Player);

		Player->StatComp->AddGold(5);

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
}