#include "Component/STInventoryComponent.h"
#include "Item/STItemDataAssetBase.h"
#include "Character/Player/STLocalPlayer.h"
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

bool USTInventoryComponent::AddItem(USTItemDataAssetBase* NewItem, int32 Count, int32& OutAddedInventoryIndex)
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
		OutAddedInventoryIndex = StackSlot;

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
		OutAddedInventoryIndex = EmptyIndex;
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

	// 퀵슬롯 업데이트
	USTQuickSlotComponent* QuickSlotComp = GetOwner()->FindComponentByClass<USTQuickSlotComponent>();
	if (QuickSlotComp)
	{
		for (int32 i = 0; i < QuickSlotComp->InventorySlotIndex.Num(); i++)
		{
			

			if (QuickSlotComp->InventorySlotIndex[i] == SlotIndex)
			{
				QuickSlotComp->QuickSlots[i].Count--;
				if (Slot.ItemData == nullptr)
				{
					QuickSlotComp->QuickSlots[i].ItemData = nullptr;
					QuickSlotComp->QuickSlots[i].Count = 0;
					QuickSlotComp->InventorySlotIndex[i] = -1;
				}
			}
		}
	}

	return true;
}

bool USTInventoryComponent::UseItem(int32 SlotIndex, ASTLocalPlayer* Player, int32 StaminaCost, FInventorySlot& OutSlot)
{
	OutSlot = FInventorySlot();

	if (!Slots.IsValidIndex(SlotIndex))
	{
		return false;
	}

	FInventorySlot& Slot = Slots[SlotIndex];

	if (!Slot.ItemData)
	{
		return false;
	}

	// UseEffectClass가 없으면 사용 불가
	if (!Slot.ItemData->UseEffectClass)
		return false;

	// 사용 전 슬롯 정보 복사(블루프린트로 전달할 값)
	FInventorySlot PreUseSlot = Slot;

	USTItemUseEffect* Effect = NewObject<USTItemUseEffect>(this, Slot.ItemData->UseEffectClass);
	if (!Effect)
	{
		return false;
	}

	// 사용 가능 검사
	if (!Effect->CanUse(Player, Slot.ItemData))
	{
		return false;
	}

	// 스태미나 차감(인자로 들어온 값 우선, 없으면 아이템 데이터 사용)
	const int Cost = (StaminaCost > 0) ? StaminaCost : Slot.ItemData->StaminaCost;
	if (Player && Player->StatComp)
	{
		Player->StatComp->AddStamina(-Cost);
	}

	// 아이템 소비 및 효과 실행
	RemoveItem(SlotIndex, 1);
	Effect->Use(Player, Slot.ItemData);

	// 인벤토리 업데이트 브로드캐스트
	OnInventoryUpdated.Broadcast();

	// 사용 전 슬롯 정보를 OutSlot에 채워서 반환
	OutSlot = PreUseSlot;
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

	// 퀵슬롯의 InventorySlotIndex 갱신
	USTQuickSlotComponent* QuickSlotComp = GetOwner()->FindComponentByClass<USTQuickSlotComponent>();
	if (QuickSlotComp)
	{
		for (int32 i = 0; i < QuickSlotComp->InventorySlotIndex.Num(); i++)
		{
			if (QuickSlotComp->InventorySlotIndex[i] == SlotAIndex)
			{
				QuickSlotComp->InventorySlotIndex[i] = SlotBIndex;
			}
			else if (QuickSlotComp->InventorySlotIndex[i] == SlotBIndex)
			{
				QuickSlotComp->InventorySlotIndex[i] = SlotAIndex;
			}
		}
	}

	// UI 갱신
	OnSlotChanged(SlotAIndex, SlotBIndex);

	// WB_Inventory에서 바인드 해둔 UpdateInventoryDrop 이벤트를 호출
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