// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/STQuickSlotComponent.h"

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

	if (!QuickSlots.IsValidIndex(TargetQuickSlotIndex))
	{
		return false;
	}

	// 인벤토리에서 아이템 가져오기
	const FInventorySlot ItemSlot = InventorySystem->Slots[InventoryItemIndex];

	// 해당 아이템이 몇번인지 저장
	InventorySlotIndex = InventoryItemIndex;

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