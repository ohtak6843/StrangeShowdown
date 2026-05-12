// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/STWheelEffect.h"
#include "Character/Player/STLocalPlayer.h"
#include "Component/STStatComponent.h"

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
	if (User->StatComp->CurrentStamina >= User->StatComp->MaxStamina)
	{
		return false;
	}

	Super::Use(User, ItemData);

	// TODO: 이동속도 증가
	User->StatComp->AddMoveSpeed(100);

	return true;
}
