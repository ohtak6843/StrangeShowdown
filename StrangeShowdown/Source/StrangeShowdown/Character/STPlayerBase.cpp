// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/STPlayerBase.h"

// Sets default values
ASTPlayerBase::ASTPlayerBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASTPlayerBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASTPlayerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASTPlayerBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

