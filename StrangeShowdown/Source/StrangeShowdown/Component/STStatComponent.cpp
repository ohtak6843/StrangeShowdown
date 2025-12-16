// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/STStatComponent.h"

// Sets default values for this component's properties
USTStatComponent::USTStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USTStatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void USTStatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void USTStatComponent::SetHp(int32 HealAmount)
{
	Hp += HealAmount;

	if (Hp <= 0)
	{
		Hp = 0;
		bAlive = false;
	}

	if(Hp > MaxHp)
	{
		Hp = MaxHp;
	}
}

void USTStatComponent::SetGold(int32 GoldAmount)
{
	Gold += GoldAmount;
}

void USTStatComponent::SetKill(int32 KillAmount)
{
	Kill += KillAmount;
}

void USTStatComponent::SetMoveSpeed(int32 MoveSpeedAmount)
{
	MoveSpeed += MoveSpeedAmount;
}

void USTStatComponent::SetStamina(int32 StaminaAmount)
{
	Stamina += StaminaAmount;
}

void USTStatComponent::SetAction(int32 ActionAmount)
{
	Action += ActionAmount;
}

void USTStatComponent::SetPrize(int32 PrizeAmount)
{
	Prize += PrizeAmount;
}

void USTStatComponent::SetIsActive(bool isActive)
{
	bAlive = isActive;
}
