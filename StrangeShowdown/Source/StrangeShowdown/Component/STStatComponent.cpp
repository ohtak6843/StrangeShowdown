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

void USTStatComponent::ResetMoveSpeed()
{
	float CommonMoveSpeed = 500.f;

	ACharacter* Owner = Cast<ACharacter>(GetOwner());
	if (Owner)
	{
		Owner->GetCharacterMovement()->MaxWalkSpeed = CommonMoveSpeed;
	}

	OnStatChanged.Broadcast();
}

void USTStatComponent::InitPlayerStats()
{
	CharacterStat.CurrentHp = Common::PlayerConstants::Hp;
	CharacterStat.MaxHp = Common::PlayerConstants::MaxHp;
	CharacterStat.CurrentGold = Common::PlayerConstants::Gold;
	// armor
	CharacterStat.KillCount = 0;
	// movespeed
	CharacterStat.CurrentStamina = Common::PlayerConstants::Stamina;
	CharacterStat.MaxStamina = Common::PlayerConstants::MaxStamina;

	OnStatChanged.Broadcast();
}


void USTStatComponent::HandleStatusUpdate(const Common::SCStatusUpdate& Packet)
{
	CharacterStat.CurrentHp = Packet.hp;
	CharacterStat.CurrentStamina = 2.f;
	CharacterStat.CurrentGold = Packet.gold;
	CharacterStat.CurrentArmor = Packet.armor;
	OnStatChanged.Broadcast();
}