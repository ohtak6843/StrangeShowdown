// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/STStatComponent.h"

// Sets default values for this component's properties
USTStatComponent::USTStatComponent()
{
	bWantsInitializeComponent = true;
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

	OnStatChanged.Broadcast();
}

void USTStatComponent::AddKill(int32 KillAmount)
{
	Kill += KillAmount;
}

void USTStatComponent::AddArmor(int32 ArmorAmount)
{
	CurrentArmor += ArmorAmount;
}

void USTStatComponent::AddMoveSpeed(int32 MoveSpeedAmount)
{
	MoveSpeed += MoveSpeedAmount;
}

void USTStatComponent::AddStamina(int32 StaminaAmount)
{
	CurrentStamina = FMath::Clamp(CurrentStamina + StaminaAmount, 0.f, MaxStamina);

	OnStatChanged.Broadcast();
}

void USTStatComponent::AddAction(int32 ActionAmount)
{
	CurrentAction = FMath::Clamp(CurrentAction + ActionAmount, 0.f, UseAbleAction);

	OnStatChanged.Broadcast();
}

void USTStatComponent::AddUseAbleAction(int32 ActionAmount)
{
	UseAbleAction += ActionAmount;
}

void USTStatComponent::AddPrize(int32 PrizeAmount)
{
	Bounty += PrizeAmount;
}

void USTStatComponent::SetIsActive(bool isActive)
{
	bAlive = isActive;
}
