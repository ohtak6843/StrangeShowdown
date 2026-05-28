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
	FSTCharacterStat& CharacterStat = User->StatComp->GetCharacterStat();
	if (CharacterStat.CurrentHp >= CharacterStat.MaxHp)
	{
		return false;
	}

	Super::Use(User, ItemData);

	User->StatComp->SetCurrentHp(CharacterStat.CurrentHp + 1);

	return true;
}
