// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/STStatComponent.h"
#include "CommonDefine.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values for this component's properties
USTStatComponent::USTStatComponent()
{
	bWantsInitializeComponent = true;
}

void USTStatComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().ClearTimer(MoveSpeedBuffTimerHandle);
	}

	Super::EndPlay(EndPlayReason);
}

void USTStatComponent::AddHp(int32 HealAmount)
{
	CurrentHp = FMath::Clamp(CurrentHp + HealAmount, 0.f, MaxHp);

	OnStatChanged.Broadcast();
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
	ACharacter* Owner = Cast<ACharacter>(GetOwner());
	if (Owner)
	{
		Owner->GetCharacterMovement()->MaxWalkSpeed += MoveSpeedAmount;
	}

	float BuffTime = 5.0f;
	UWorld* World = GetWorld();
	if (World)
	{
		FTimerDelegate TimerDel;
		TimerDel.BindUObject(this, &USTStatComponent::ResetMoveSpeed);
		World->GetTimerManager().SetTimer(MoveSpeedBuffTimerHandle, TimerDel, BuffTime, false);
	}

	MoveSpeed += MoveSpeedAmount;

	OnStatChanged.Broadcast();
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

void USTStatComponent::ResetMoveSpeed()
{
	float CommonMoveSpeed = 500.f;

	ACharacter* Owner = Cast<ACharacter>(GetOwner());
	if (Owner)
	{
		Owner->GetCharacterMovement()->MaxWalkSpeed = CommonMoveSpeed;
	}

	MoveSpeed = CommonMoveSpeed;
	OnStatChanged.Broadcast();
}

void USTStatComponent::InitPlayerStats()
{
	CurrentHp = Common::PlayerConstants::Hp;
	MaxHp = Common::PlayerConstants::MaxHp;
	Gold = Common::PlayerConstants::Gold;
	// armor
	Kill = 0;
	// movespeed
	CurrentStamina = Common::PlayerConstants::Stamina;
	MaxStamina = Common::PlayerConstants::MaxStamina;

	OnStatChanged.Broadcast();
}
