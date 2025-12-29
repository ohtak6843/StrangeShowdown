#include "Component/STStoreComponent.h"
#include "Player/STLocalPlayer.h"
#include "Component/STStatComponent.h"
#include "Component/STInventoryComponent.h"
#include "Item/STItemDataAssetBase.h"

USTStoreComponent::USTStoreComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USTStoreComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerPlayer = Cast<ASTLocalPlayer>(GetOwner());

	Slots.SetNum(SlotCount);
	StoreItemPool.SetNum(SlotCount);
}

void USTStoreComponent::InitStore()
{
	if (StoreItemPool.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("StoreItemPool is empty"));
		return;
	}

	for (int32 i = 0; i < Slots.Num(); i++)
	{
		Slots[i].ItemData = StoreItemPool[0];
		Slots[i].SlotIndex = i;
		Slots[i].bSold = false;
	}
}

bool USTStoreComponent::BuyItem(int32 Index)
{
	if (!Slots.IsValidIndex(Index))
		return false;

	if (Slots[Index].bSold)
		return false;

	if (!OwnerPlayer || !OwnerPlayer->StatComp || !OwnerPlayer->InventoryComp)
		return false;

	USTItemDataAssetBase* ItemData = Slots[Index].ItemData;
	if (!ItemData)
		return false;

	OwnerPlayer->StatComp->AddGold(-ItemData->GoldCost);
	OwnerPlayer->InventoryComp->AddItem(ItemData, 1);

	Slots[Index].bSold = true;
	return true;
}

void USTStoreComponent::Reroll()
{
	if (!OwnerPlayer || !OwnerPlayer->StatComp)
		return;

	OwnerPlayer->StatComp->AddGold(-RerollCost);
	InitStore();
}