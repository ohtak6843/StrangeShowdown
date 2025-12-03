// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/STInventoryComponent.h"
#include "Item/STItemDataAssetBase.h"

// Sets default values for this component's properties
USTInventoryComponent::USTInventoryComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;

    // ...
}


// Called when the game starts
void USTInventoryComponent::BeginPlay()
{
    Super::BeginPlay();

    Items.SetNum(MaxSlots);
}


// Called every frame
void USTInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // ...
}

bool USTInventoryComponent::AddItem(USTItemDataAssetBase* NewItem, int32 Count)
{
    if (NewItem == nullptr || Count <= 0)
        return false;

    // 스택 가능한 슬롯 찾기
    int32 StackSlot = FindStackableSlot(NewItem);
    if (StackSlot != -1)
    {
        int32 SpaceLeft = NewItem->MaxStack - Items[StackSlot].Count;
        int32 AddCount = FMath::Min(SpaceLeft, Count);

        Items[StackSlot].Count += AddCount;
        Count -= AddCount;

        UE_LOG(LogTemp, Warning, TEXT("[Inventory] Stacked %d of %s in Slot %d (Now: %d)"),
            AddCount,
            *NewItem->ItemName.ToString(),
            StackSlot,
            Items[StackSlot].Count
        );

        if (Count <= 0)
            return true;
    }

    // 빈 슬롯에 새로 추가
    while (Count > 0)
    {
        int32 EmptyIndex = FindEmptySlot();
        if (EmptyIndex == -1)
        {
            UE_LOG(LogTemp, Error, TEXT("[Inventory] Failed to add %s - Inventory full"),
                *NewItem->ItemName.ToString());
            return false;
        }

        int32 AddCount = FMath::Min(Count, NewItem->MaxStack);

        Items[EmptyIndex].ItemData = NewItem;
        Items[EmptyIndex].Count = AddCount;

        UE_LOG(LogTemp, Warning, TEXT("[Inventory] Added New Item %s (Slot: %d, Count: %d)"),
            *NewItem->ItemName.ToString(),
            EmptyIndex,
            AddCount
        );

        Count -= AddCount;
    }

    return true;
}

bool USTInventoryComponent::RemoveItem(int32 SlotIndex, int32 Count)
{
    if (!Items.IsValidIndex(SlotIndex))
        return false;

    if (Count <= 0)
        return false;

    FInventorySlot& Slot = Items[SlotIndex];

    if (Slot.ItemData == nullptr)
        return false;

    // 개수 차감
    Slot.Count -= Count;

    // 0개 이하가 되면 비우기
    if (Slot.Count <= 0)
    {
        Slot.ItemData = nullptr;
        Slot.Count = 0;
    }

    return true;
}

int32 USTInventoryComponent::FindEmptySlot() const
{
    for (int32 i = 0; i < Items.Num(); i++)
    {
        if (Items[i].ItemData == nullptr)
            return i;
    }
    return -1;
}

int32 USTInventoryComponent::FindStackableSlot(USTItemDataAssetBase* NewItem) const
{
    if (NewItem == nullptr)
        return -1;

    for (int32 i = 0; i < Items.Num(); i++)
    {
        if (Items[i].ItemData == NewItem &&
            Items[i].Count < NewItem->MaxStack)
        {
            return i;
        }
    }
    return -1;
}

