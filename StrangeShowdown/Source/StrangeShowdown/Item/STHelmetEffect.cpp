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
	FSTCharacterStat& CharacterStat = User->StatComp->GetCharacterStat();
	if (CharacterStat.CurrentArmor >= CharacterStat.MaxArmor)
	{
		return false;
	}

	Super::Use(User, ItemData);

	float ArmorAmount = 3.f;
	User->StatComp->SetCurrentArmor(CharacterStat.CurrentArmor + ArmorAmount);

	return true;
}
