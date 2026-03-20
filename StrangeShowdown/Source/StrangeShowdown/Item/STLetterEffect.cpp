// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/STLetterEffect.h"
#include "Character/Player/STLocalPlayer.h"
#include "Component/STMissionComponent.h"

USTLetterEffect::USTLetterEffect()
	: Super()
{
	static ConstructorHelpers::FObjectFinder<USoundBase> SoundAsset(
		TEXT("/Script/Engine.SoundWave'/Game/StrangeShowdown/Sound/Effect/SW_Mission.SW_Mission'")
	);
	if (SoundAsset.Succeeded())
	{
		UseSound = SoundAsset.Object;
	}
}

bool USTLetterEffect::Use(ASTLocalPlayer* User, USTItemDataAssetBase* ItemData)
{
	if (!User) return false;

	auto MissionComp = User->GetMissionComp();
	if (!MissionComp) return false;

	Super::Use(User, ItemData);

	MissionComp->SetMission(
		FText::FromString(TEXT("테스트 미션")),
		FText::FromString(TEXT("이것은 테스트 미션입니다. 다시 한번 말씀드립니다. 테스트 미션입니다."))
	);

	return true;
}
