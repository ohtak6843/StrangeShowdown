// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/STMineral.h"
#include "Character/Player/STPlayerBase.h"

ASTMineral::ASTMineral()
{
	SubMineralMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SubMineralMeshComponent"));
	SubMineralMeshComponent->SetupAttachment(RootComponent);
}

void ASTMineral::BeginPlay()
{
	Super::BeginPlay();
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
		Player->StatComp->AddGold(5);
		Super::Slice(HitLocation, HitNormal, Player);
		return;
	}
}