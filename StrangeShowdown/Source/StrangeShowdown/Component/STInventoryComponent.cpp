#include "Component/STInventoryComponent.h"
#include "Item/STItemDataAssetBase.h"
#include "STQuickSlotComponent.h"

USTInventoryComponent::USTInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USTInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	Slots.SetNum(MaxSlots);

	// MouseDrop이 발생하면 ChangeSlot 실행되도록 바인딩
	MouseDrop.AddDynamic(this, &USTInventoryComponent::ChangeSlot_FromEvent);
}

void USTInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool USTInventoryComponent::AddItem(USTItemDataAssetBase* NewItem, int32 Count)
{
	if (!NewItem || Count <= 0)
		return false;

	int32 StackSlot = FindStackableSlot(NewItem);

	if (StackSlot != -1)
	{
		int32 SpaceLeft = NewItem->MaxStack - Slots[StackSlot].Count;
		int32 AddCount = FMath::Min(SpaceLeft, Count);

		Slots[StackSlot].Count += AddCount;
		Count -= AddCount;

		if (Count <= 0)
			return true;
	}

	while (Count > 0)
	{
		int32 EmptyIndex = FindEmptySlot();
		if (EmptyIndex == -1)
			return false;

		int32 AddCount = FMath::Min(Count, NewItem->MaxStack);

		Slots[EmptyIndex].ItemData = NewItem;
		Slots[EmptyIndex].Count = AddCount;

		Count -= AddCount;
	}

	return true;
}

bool USTInventoryComponent::RemoveItem(int32 SlotIndex, int32 Count)
{
	if (!Slots.IsValidIndex(SlotIndex) || Count <= 0)
		return false;

	FInventorySlot& Slot = Slots[SlotIndex];

	if (!Slot.ItemData)
		return false;

	Slot.Count -= Count;

	if (Slot.Count <= 0)
	{
		Slot.ItemData = nullptr;
		Slot.Count = 0;
	}

	return true;
}

bool USTInventoryComponent::UseItem(int32 SlotIndex, ASTLocalPlayer* Player)
{
	if (!Slots.IsValidIndex(SlotIndex))
	{
		return false;
	}

	FInventorySlot& Slot = Slots[SlotIndex];

	if (!Slot.ItemData)
	{
		return false;
	}

	Slot.ItemData->UseItem(Player);

	RemoveItem(SlotIndex, 1);

	// 인벤토리 업데이트
	OnInventoryUpdated.Broadcast();

	return true;
}

bool USTInventoryComponent::ChangeSlot(int32 SlotAIndex, int32 SlotBIndex, USTInventoryComponent* BeforeInventorySystem)
{
	if (!Slots.IsValidIndex(SlotAIndex) || !Slots.IsValidIndex(SlotBIndex))
		return false;

	FInventorySlot& SlotA = Slots[SlotAIndex];
	FInventorySlot& SlotB = Slots[SlotBIndex];

	if (BeforeInventorySystem)
	{
		FInventorySlot Temp = SlotA;
		SlotA = BeforeInventorySystem->Slots[SlotBIndex];
		BeforeInventorySystem->Slots[SlotBIndex] = Temp;
	}
	else
	{
		FInventorySlot Temp = SlotA;
		SlotA = SlotB;
		SlotB = Temp;
	}

	// UI 갱신
	OnSlotChanged(SlotAIndex, SlotBIndex);

	// 인벤토리 업데이트
	OnInventoryUpdated.Broadcast();

	if (BeforeInventorySystem)
	{
		BeforeInventorySystem->OnInventoryUpdated.Broadcast();
	}

	return true;
}

void USTInventoryComponent::CallMouseDrop(int32 TargetIndex, USTInventoryComponent* BeforeInventorySystem, int32 BeforeIndex)
{
	MouseDrop.Broadcast(TargetIndex, BeforeInventorySystem, BeforeIndex);
}

void USTInventoryComponent::ChangeSlot_FromEvent(int32 TargetIndex, USTInventoryComponent* BeforeInventorySystem, int32 BeforeIndex)
{
	ChangeSlot(TargetIndex, BeforeIndex, BeforeInventorySystem);
}

int32 USTInventoryComponent::FindEmptySlot() const
{
	for (int32 i = 0; i < Slots.Num(); i++)
	{
		if (Slots[i].ItemData == nullptr)
			return i;
	}
	return -1;
}

int32 USTInventoryComponent::FindStackableSlot(USTItemDataAssetBase* NewItem) const
{
	if (!NewItem)
		return -1;

	for (int32 i = 0; i < Slots.Num(); i++)
	{
		if (Slots[i].ItemData == NewItem &&
			Slots[i].Count < NewItem->MaxStack)
		{
			return i;
		}
	}
	return -1;
}