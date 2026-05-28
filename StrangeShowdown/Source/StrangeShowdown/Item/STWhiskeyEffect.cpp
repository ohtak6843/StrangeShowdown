// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/STWhiskeyEffect.h"
#include "Character/Player/STLocalPlayer.h"
#include "Component/STStatComponent.h"

USTWhiskeyEffect::USTWhiskeyEffect()
	: Super()
{
	static ConstructorHelpers::FObjectFinder<USoundBase> SoundAsset(
		TEXT("/Script/Engine.SoundWave'/Game/StrangeShowdown/Sound/Effect/SW_DrinkWhiskey.SW_DrinkWhiskey'")
	);
	if (SoundAsset.Succeeded())
	{
		UseSound = SoundAsset.Object;
	}
}

bool USTWhiskeyEffect::Use(ASTLocalPlayer* User, USTItemDataAssetBase* ItemData)
{
	FSTCharacterStat& CharacterStat = User->StatComp->GetCharacterStat();
	if (CharacterStat.CurrentStamina >= CharacterStat.MaxStamina)
	{
		return false;
	}

	Super::Use(User, ItemData);

	float StaminaBuffAmount = 1.f;
	CharacterStat.SetCurrentStamina(CharacterStat.CurrentStamina + StaminaBuffAmount);

	return true;
}
