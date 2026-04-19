// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/STSlotWidget.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Item/STItemDataAssetBase.h"
#include "UI/Inventory/STTooltipWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "UI/Inventory/STDragWidget.h"
#include "DragDropOperation/STInventoryDragDropOperation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Component/STInventoryComponent.h"

USTSlotWidget::USTSlotWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void USTSlotWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	if (nullptr == Tooltip && SlotData.ItemData)
	{
		Tooltip = CreateWidget<USTTooltipWidget>(GetWorld(), TooltipWidgetClass);
		Tooltip->SetVisibility(ESlateVisibility::HitTestInvisible);
		Tooltip->UpdateTooltip(SlotData);

		FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(this);
		FVector2D SnappedPosition = MousePosition.RoundToVector().IntPoint();
		Tooltip->SetPositionInViewport(SnappedPosition, false);

		Tooltip->AddToViewport();
	}
}

FReply USTSlotWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply ReplyResult = Super::NativeOnMouseMove(InGeometry, InMouseEvent);

	if (Tooltip)
	{
		FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(this);
		FVector2D SnappedPosition = MousePosition.RoundToVector().IntPoint();
		Tooltip->SetPositionInViewport(SnappedPosition, false);
	}

	return ReplyResult;
}

void USTSlotWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	if (Tooltip)
	{
		Tooltip->RemoveFromParent();
		Tooltip = nullptr;
	}
}

FReply USTSlotWidget::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Result = Super::NativeOnPreviewMouseButtonDown(InGeometry, InMouseEvent);

	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		Result = FReply::Handled().DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
	}

	return Result;
}

void USTSlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	if (nullptr == SlotData.ItemData)
	{
		return;
	}

	USTDragWidget* DragWidget = CreateWidget<USTDragWidget>(GetOwningPlayer(), DragWidgetClass);
	if (DragWidget)
	{
		DragWidget->SetDragImage(SlotData.ItemData->Icon);
	}

	USTInventoryDragDropOperation* DragDropOperation = Cast<USTInventoryDragDropOperation>(UWidgetBlueprintLibrary::CreateDragDropOperation(USTInventoryDragDropOperation::StaticClass()));
	if (DragDropOperation)
	{
		DragDropOperation->DefaultDragVisual = DragWidget;
		DragDropOperation->Pivot = EDragPivot::CenterCenter;
		DragDropOperation->SourceInventory = GetOwningPlayerPawn()->FindComponentByClass<USTInventoryComponent>();
		DragDropOperation->SourceSlotIndex = SlotIndex;

		OutOperation = DragDropOperation;
	}
}

bool USTSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	USTInventoryDragDropOperation* DragDropOperation = Cast<USTInventoryDragDropOperation>(InOperation);
	if (DragDropOperation)
	{
		bool bDifferentIndex = DragDropOperation->SourceSlotIndex != SlotIndex;
		UE_LOG(LogTemp, Log, TEXT("Source Slot Index: %d, Target Slot Index: %d"), DragDropOperation->SourceSlotIndex, SlotIndex);
		bool bDifferentInventory = DragDropOperation->SourceInventory != GetOwningPlayerPawn()->FindComponentByClass<USTInventoryComponent>();

		if (bDifferentIndex || bDifferentInventory)
		{
			DragDropOperation->SourceInventory->ChangeSlot(DragDropOperation->SourceSlotIndex, SlotIndex, DragDropOperation->SourceInventory);
		}

		return true;
	}

	return false;
}

void USTSlotWidget::UpdateSlot(const FSTItemSlot& ItemSlot, int32 Index)
{
	SlotData = ItemSlot;
	SlotIndex = Index;

	if (SlotData.ItemData)
	{
		ItemImage->SetBrushFromTexture(SlotData.ItemData->Icon);
		ItemCount->SetText(FText::AsNumber(SlotData.Count));
		ItemImage->SetVisibility(ESlateVisibility::Visible);
		ItemCountBox->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		ItemImage->SetVisibility(ESlateVisibility::Hidden);
		ItemCountBox->SetVisibility(ESlateVisibility::Hidden);
	}
}
