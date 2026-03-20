// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/STHelmetEffect.h"
#include "Character/Player/STLocalPlayer.h"
#include "Component/STStatComponent.h"

USTHelmetEffect::USTHelmetEffect()
	: Super()
{
	static ConstructorHelpers::FObjectFinder<USoundBase> SoundAsset(
		TEXT("/Script/Engine.SoundWave'/Game/StrangeShowdown/Sound/Effect/SW_UseHelmet.SW_UseHelmet'")
	);
	if (SoundAsset.Succeeded())
	{
		UseSound = SoundAsset.Object;
	}
}

bool USTHelmetEffect::Use(ASTLocalPlayer* User, USTItemDataAssetBase* ItemData)
{
	if (User->StatComp->CurrentArmor >= User->StatComp->MaxArmor)
	{
		return false;
	}

	Super::Use(User, ItemData);

	User->StatComp->AddArmor(3);

	return true;
}
