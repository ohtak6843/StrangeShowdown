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
	CurrentHp += HealAmount;

	if (CurrentHp <= 0)
	{
		CurrentHp = 0;
		bAlive = false;
	}

	if(CurrentHp > MaxHp)
	{
		CurrentHp = MaxHp;
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
	CurrentStamina += StaminaAmount;

	if (CurrentStamina > MaxStamina)
	{
		CurrentStamina = MaxStamina;
	}
	else if (CurrentStamina <= 0)
	{
		CurrentStamina = 0;
	}
}

void USTStatComponent::AddAction(int32 ActionAmount)
{
	CurrentAction += ActionAmount;
}

void USTStatComponent::AddUseAbleAction(int32 ActionAmount)
{
	UseAbleAction += ActionAmount;
}

void USTStatComponent::AddPrize(int32 PrizeAmount)
{
	Prize += PrizeAmount;
}

void USTStatComponent::SetIsActive(bool isActive)
{
	bAlive = isActive;
}
