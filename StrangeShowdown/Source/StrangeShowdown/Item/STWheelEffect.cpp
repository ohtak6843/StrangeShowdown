// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/STWheelEffect.h"
#include "Character/Player/STLocalPlayer.h"
#include "Component/STStatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

USTWheelEffect::USTWheelEffect()
	: Super()
{
	static ConstructorHelpers::FObjectFinder<USoundBase> SoundAsset(
		TEXT("/Script/Engine.SoundWave'/Game/StrangeShowdown/Sound/Effect/SW_Wheel.SW_Wheel'")
	);
	if (SoundAsset.Succeeded())
	{
		UseSound = SoundAsset.Object;
	}
}

bool USTWheelEffect::Use(ASTLocalPlayer* User, USTItemDataAssetBase* ItemData)
{
	FSTCharacterStat& CharacterStat = User->StatComp->GetCharacterStat();
	if (CharacterStat.CurrentStamina >= CharacterStat.MaxStamina)
	{
		return false;
	}

	Super::Use(User, ItemData);

	float SpeedBuffAmount = 100.f;
	User->SetMaxWalkSpeed(User->GetCharacterMovement()->MaxWalkSpeed + SpeedBuffAmount);

	return true;
}
