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

void USTStatComponent::AddHp(int32 HealAmount)
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

void USTStatComponent::AddGold(int32 GoldAmount)
{
	Gold += GoldAmount;
}

void USTStatComponent::AddKill(int32 KillAmount)
{
	Kill += KillAmount;
}

void USTStatComponent::AddPower(int32 PowerAmount)
{
	Power += PowerAmount;
}

void USTStatComponent::AddArmor(int32 ArmorAmount)
{
	Armor += ArmorAmount;
}

void USTStatComponent::AddMoveSpeed(int32 MoveSpeedAmount)
{
	MoveSpeed += MoveSpeedAmount;
}

void USTStatComponent::AddStamina(int32 StaminaAmount)
{
	Stamina += StaminaAmount;

	if (Stamina > MaxStamina)
	{
		Stamina = MaxStamina;
	}
}

void USTStatComponent::AddAction(int32 ActionAmount)
{
	Action += ActionAmount;
}

void USTStatComponent::AddPrize(int32 PrizeAmount)
{
	Prize += PrizeAmount;
}

void USTStatComponent::SetIsActive(bool isActive)
{
	bAlive = isActive;
}
