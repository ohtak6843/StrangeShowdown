// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/STItemDataAssetBase.h"

USTItemDataAssetBase::USTItemDataAssetBase()
{

}

void USTItemDataAssetBase::UseItem(ASTLocalPlayer* Player)
{
	// 타입별 처리
	switch (ItemType)
	{
	case EItemType::HealPack:
		// HealPack 사용 로직
		break;
	case EItemType::WeaponEnforce:
		// WeaponEnforce 사용 로직
		break;
	case EItemType::WirelessSet:
		// WirelessSet 사용 로직
		break;
	case EItemType::BodyArmor:
		// BodyArmor 사용 로직
		break;
	case EItemType::Temp:
		// 임시용 로그
		UE_LOG(LogTemp, Warning, TEXT("Unknown item type used."));
		break;
	}
}