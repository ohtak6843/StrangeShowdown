// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/STItemUseEffect.h"
#include "Character/Player/STLocalPlayer.h"
#include "Item/STItemDataAssetBase.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Kismet/GameplayStatics.h"

USTItemUseEffect::USTItemUseEffect()
{
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> EffectRef(TEXT("/Script/Niagara.NiagaraSystem'/Game/StrangeShowdown/Item/FX/NS_UseEffect.NS_UseEffect'"));
	if (EffectRef.Object)
	{
		HitEffect = EffectRef.Object;
	}
}

bool USTItemUseEffect::Use(ASTLocalPlayer* User, USTItemDataAssetBase* ItemData)
{
	if (HitEffect && User)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(User->GetWorld(), HitEffect, User->GetActorLocation(), FRotator::ZeroRotator);
	}
	
	if (UseSound && User)
	{
		UGameplayStatics::PlaySoundAtLocation(User->GetWorld(), UseSound, User->GetActorLocation());
	}
	return true;
}

bool USTItemUseEffect::CanUse(ASTLocalPlayer* User, USTItemDataAssetBase* ItemData)
{
	FSTCharacterStat& CharacterStat = User->StatComp->GetCharacterStat();
	if (ItemData->StaminaCost > CharacterStat.CurrentStamina)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot use item due to insufficient stamina."));
		return false;
	}
	return true;
}