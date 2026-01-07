// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/STQuickSlotComponent.h"
#include "Item/STItemDataAssetBase.h"

// Sets default values for this component's properties
USTQuickSlotComponent::USTQuickSlotComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USTQuickSlotComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	// Resize QuickSlots array
	QuickSlots.SetNum(QuickSlotSize);
	InventorySlotIndex.SetNum(QuickSlotSize);
	
	// InventorySlotIndex 초기화
	for(int32 i = 0; i < InventorySlotIndex.Num(); i++)
	{
		InventorySlotIndex[i] = -1;
	}

	MouseDropToQuickSlot.AddDynamic(this, &USTQuickSlotComponent::AddToQuickSlot_FromEvent);
}


// Called every frame
void USTQuickSlotComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool USTQuickSlotComponent::AddToQuickSlot(USTInventoryComponent* InventorySystem, int32 InventoryItemIndex, int32 TargetQuickSlotIndex)
{
	if (InventorySystem == nullptr)
	{
		return false;
	}

	// 인벤토리 슬롯 유효성 검사
	if (!InventorySystem->Slots.IsValidIndex(InventoryItemIndex))
	{
		return false;
	}

	// 인벤토리에서 아이템 가져오기
	FInventorySlot ItemSlot = InventorySystem->Slots[InventoryItemIndex];

	// TargetQuickSlotIndex가 -2면 빈 슬롯에 추가(바로 추가)
	if (TargetQuickSlotIndex == -2)
	{
		// 만약 퀵슬롯에 같은 아이템이 있으면 패스
		for (int32 i = 1; i < QuickSlots.Num(); i++)
		{
			if (QuickSlots[i].ItemData == ItemSlot.ItemData)
			{
				return false;
			}
		}

		// 빈 슬롯 찾기
		for (int32 i = 1; i < QuickSlots.Num(); i++)
		{
			if (QuickSlots[i].ItemData == nullptr)
			{
				TargetQuickSlotIndex = i;
				break;
			}
		}

		// 빈 슬롯을 못 찾으면 실패
		if (TargetQuickSlotIndex == -2)
		{
			return false;
		}
	}

	// TargetQuickSlotIndex가 유효한 인덱스인지 검사
	if (!QuickSlots.IsValidIndex(TargetQuickSlotIndex))
	{
		return false;
	}

	// 1번 슬롯은 무기가 고정되어야 함
	if (TargetQuickSlotIndex == 0)
	{
		return false;
	}

	// 해당 아이템이 몇번인지 저장
	InventorySlotIndex[TargetQuickSlotIndex] = InventoryItemIndex;

	// 퀵슬롯에 설정 (Set Array Elem)
	QuickSlots[TargetQuickSlotIndex] = ItemSlot;

	// Call On QuickSlotUpdated
	OnQuickSlotUpdated.Broadcast();
	return true;
}

void USTQuickSlotComponent::CallMouseDropToQuickSlot(int32 InventoryItemIndex, USTInventoryComponent* BeforeInventorySystem, int32 QuickSlotIndex)
{
	MouseDropToQuickSlot.Broadcast(InventoryItemIndex, BeforeInventorySystem, QuickSlotIndex);
}

void USTQuickSlotComponent::AddToQuickSlot_FromEvent(int32 InventoryItemIndex, USTInventoryComponent* BeforeInventorySystem, int32 QuickSlotIndex)
{
	AddToQuickSlot(BeforeInventorySystem, InventoryItemIndex, QuickSlotIndex);
	
	OnQuickSlotUpdated.Broadcast();
}