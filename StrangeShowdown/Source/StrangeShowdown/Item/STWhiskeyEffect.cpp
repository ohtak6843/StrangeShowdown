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
	if (User->StatComp->CurrentStamina >= User->StatComp->MaxStamina)
	{
		return false;
	}

	Super::Use(User, ItemData);

	User->StatComp->AddStamina(1);

	return true;
}
