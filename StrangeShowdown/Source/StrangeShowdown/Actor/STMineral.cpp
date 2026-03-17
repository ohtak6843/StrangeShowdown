// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/STMineral.h"

ASTMineral::ASTMineral()
{
	SubMineralMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SubMineralMeshComponent"));
	SubMineralMeshComponent->SetupAttachment(RootComponent);
}

void ASTMineral::BeginPlay()
{
	Super::BeginPlay();
}

void ASTMineral::Slice(const FVector& HitLocation, const FVector& HitNormal)
{
	Super::Slice(HitLocation, HitNormal);


}