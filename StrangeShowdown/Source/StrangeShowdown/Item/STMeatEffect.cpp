// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/STMeatEffect.h"
#include "Character/Player/STLocalPlayer.h"
#include "Component/STStatComponent.h"

USTMeatEffect::USTMeatEffect()
	: Super()
{
	static ConstructorHelpers::FObjectFinder<USoundBase> SoundAsset(
		TEXT("/Script/Engine.SoundWave'/Game/StrangeShowdown/Sound/Effect/SW_EatMeat.SW_EatMeat'")
	);
	if (SoundAsset.Succeeded())
	{
		UseSound = SoundAsset.Object;
	}
}

bool USTMeatEffect::Use(ASTLocalPlayer* User, USTItemDataAssetBase* ItemData)
{
	if (User->StatComp->CurrentHp >= User->StatComp->MaxHp)
	{
		return false;
	}

	Super::Use(User, ItemData);

	User->StatComp->AddHp(1);

	return true;
}
