#include "Component/STStoreComponent.h"
#include "Character/Player/STLocalPlayer.h"
#include "Component/STStatComponent.h"
#include "Component/STInventoryComponent.h"
#include "Item/STItemDataAssetBase.h"

USTStoreComponent::USTStoreComponent()
{
	SlotCount = 5;
	RerollCost = 5;
}

void USTStoreComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerPlayer = Cast<ASTLocalPlayer>(GetOwner());

	Slots.SetNum(SlotCount);
}

void USTStoreComponent::InitStore()
{
	if (!CurrentStorekeeper.IsValid()) return;

	const TArray<USTItemDataAssetBase*>& Items = CurrentStorekeeper->StoreItemPool;

	for (int32 i = 0; i < Slots.Num(); ++i)
	{
		Slots[i].SlotIndex = i;
		Slots[i].bIsSold = false;

		if (Items.IsValidIndex(i))
		{
			Slots[i].ItemData = Items[i];
		}
		else
		{
			Slots[i].ItemData = nullptr;
		}
	}
}

void USTStoreComponent::Reroll()
{
	if (!OwnerPlayer || !OwnerPlayer->StatComp)
		return;

	OwnerPlayer->StatComp->AddGold(-RerollCost);
	InitStore();
}

void USTStoreComponent::BuyItem(int32 SlotIndex)
{
	if (CurrentStorekeeper.IsValid())
	{
		CurrentStorekeeper.Get()->BuyItem(SlotIndex);
	}
}

void USTStoreComponent::RefreshStoreUI()
{
	OnStoreUpdated.Broadcast();
}