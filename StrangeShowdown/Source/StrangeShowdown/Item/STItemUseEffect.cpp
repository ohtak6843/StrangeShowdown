// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/STItemUseEffect.h"
#include "Player/STLocalPlayer.h"
#include "Item/STItemDataAssetBase.h"
#include "STItemUseEffect.h"

bool USTItemUseEffect::CanUse(ASTLocalPlayer* User, USTItemDataAssetBase* ItemData)
{
	if (ItemData->StaminaCost > User->StatComp->CurrentStamina)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot use item due to insufficient stamina."));
		return false;
	}
	return true;
}

void USTItemUseEffect::Use_Implementation(ASTLocalPlayer* User, USTItemDataAssetBase* ItemData)
{

}