// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/STEnhancePowerEffect.h"
#include "Character/Player/STLocalPlayer.h"
#include "Component/STStatComponent.h"

USTEnhancePowerEffect::USTEnhancePowerEffect()
	: Super()
{
	static ConstructorHelpers::FObjectFinder<USoundBase> SoundAsset(
		TEXT("/Script/Engine.SoundWave'/Game/StrangeShowdown/Sound/Effect/SW_EnhancePower.SW_EnhancePower'")
	);
	if (SoundAsset.Succeeded())
	{
		UseSound = SoundAsset.Object;
	}
}

bool USTEnhancePowerEffect::Use(ASTLocalPlayer* User, USTItemDataAssetBase* ItemData)
{
	FSTCharacterStat& CharacterStat = User->StatComp->GetCharacterStat();
	if (CharacterStat.CurrentAction >= CharacterStat.MaxAction)
	{
		return false;
	}

	Super::Use(User, ItemData);

	float ActionBuffAmount = 1.f;
	User->StatComp->SetUsableAction(CharacterStat.UsableAction + ActionBuffAmount);

	return true;
}
