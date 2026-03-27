// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/STInventoryWidget.h"
#include "Component/STInventoryComponent.h"
#include "Components/WrapBox.h"
#include "UI/Inventory/STSlotWidget.h"
#include "UI/Inventory/STTooltipWidget.h"

USTInventoryWidget::USTInventoryWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<USTSlotWidget> SlotWidgetRef(TEXT("/Game/StrangeShowdown/UITest/Inventory/WBP_Slot.WBP_Slot_C"));
	if (SlotWidgetRef.Class)
	{
		SlotWidgetClass = SlotWidgetRef.Class;
	}
}

void USTInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SlotWrapBox = Cast<UWrapBox>(GetWidgetFromName(TEXT("WrapBoxSlot")));
	ensure(SlotWrapBox);

	Tooltip = Cast<USTTooltipWidget>(GetWidgetFromName(TEXT("WidgetTooltip")));
	ensure(Tooltip);
}

void USTInventoryWidget::SetupInventory(int32 InventorySlotCount)
{
	for (int32 i = 0; i < InventorySlotCount; i++)
	{
		USTSlotWidget* SlotWidget = CreateWidget<USTSlotWidget>(this, SlotWidgetClass);
		if (SlotWidget)
		{
			SlotWidget->OnSlotMouseEnter.BindUObject(this, &USTInventoryWidget::HandleSlotMouseEnter);
			SlotWidget->OnSlotMouseMove.BindUObject(this, &USTInventoryWidget::HandleSlotMouseMove);
			SlotWidget->OnSlotMouseLeave.BindUObject(this, &USTInventoryWidget::HandleSlotMouseLeave);

			SlotWrapBox->AddChild(SlotWidget);
		}
	}
}

void USTInventoryWidget::UpdateInventory(const TArray<FSTItemSlot>& InItemSlots)
{
	int32 InventorySlotWidgetCount = SlotWrapBox->GetChildrenCount();
	int32 InventoryItemCount = InItemSlots.Num();

	if (InventorySlotWidgetCount != InventoryItemCount)
	{
		SlotWrapBox->ClearChildren();
		SetupInventory(InventoryItemCount);
	}

	for (int i = 0; i < SlotWrapBox->GetChildrenCount(); i++)
	{
		USTSlotWidget* SlotWidget = Cast<USTSlotWidget>(SlotWrapBox->GetChildAt(i));
		if (SlotWidget)
		{
			SlotWidget->UpdateSlot(InItemSlots[i]);
		}
	}
}

void USTInventoryWidget::HandleSlotMouseEnter(const FSTItemSlot& ItemSlot)
{
	if (ItemSlot.ItemData)
	{
		Tooltip->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		Tooltip->UpdateTooltip(ItemSlot);
	}
}

void USTInventoryWidget::HandleSlotMouseMove(const FPointerEvent& InMouseEvent)
{
	Tooltip->SetPositionInViewport(FVector2D(InMouseEvent.GetScreenSpacePosition()));
}

void USTInventoryWidget::HandleSlotMouseLeave()
{
	Tooltip->SetVisibility(ESlateVisibility::Hidden);
}
