// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/QuickSlot/STQuickSlotWidget.h"
#include "Components/SizeBox.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Item/STItemDataAssetBase.h"
#include "Component/STInventoryComponent.h"
#include "DragDropOperation/STInventoryDragDropOperation.h"

USTQuickSlotWidget::USTQuickSlotWidget(const FObjectInitializer& ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UTexture2D> LockImageRef(TEXT("/Script/Engine.Texture2D'/Game/StrangeShowdown/UI/Texture/T_Lock.T_Lock'"));
	if (LockImageRef.Object)
	{
		LockImage = LockImageRef.Object;
	}
}

void USTQuickSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SelectBorder = Cast<UBorder>(GetWidgetFromName(TEXT("BorderSelected")));
	ensure(SelectBorder);

	ItemImage = Cast<UImage>(GetWidgetFromName(TEXT("ImageItem")));
	ensure(ItemImage);

	ItemCountBox = Cast<USizeBox>(GetWidgetFromName(TEXT("SizeBoxItemCount")));
	ensure(ItemCountBox);

	ItemCount = Cast<UTextBlock>(GetWidgetFromName(TEXT("TextItemCount")));
	ensure(ItemCount);

	SlotIndexText = Cast<UTextBlock>(GetWidgetFromName(TEXT("TextSlotIndex")));
	ensure(SlotIndexText);

	SlotIndexText->SetText(FText::AsNumber(SlotIndex + 1));
}

bool USTQuickSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	USTInventoryDragDropOperation* DragDropOperation = Cast<USTInventoryDragDropOperation>(InOperation);
	if (DragDropOperation)
	{
		USTInventoryComponent* InventoryComp = DragDropOperation->SourceInventory;
		if (OnQuickSlotWidgetDrop.IsBound())
		{
			OnQuickSlotWidgetDrop.Execute(InventoryComp, DragDropOperation->SourceSlotIndex, SlotIndex);
		}
	}

	return true;
}

void USTQuickSlotWidget::UpdateQuickSlot(const FSTItemSlot& ItemSlot, int32 CurrentSelectedIndex)
{
	SlotData = ItemSlot;

	if (nullptr == SlotData.ItemData)
	{
		ItemImage->SetVisibility(ESlateVisibility::Hidden);
		ItemCountBox->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		ItemImage->SetBrushFromTexture(SlotData.ItemData->Icon);
		ItemImage->SetVisibility(ESlateVisibility::Visible);
		
		if (SlotData.bIsCountable)
		{
			ItemCount->SetText(FText::AsNumber(SlotData.Count));
			ItemCountBox->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			ItemCountBox->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if(CurrentSelectedIndex == SlotIndex)
	{
		SelectBorder->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		SelectBorder->SetVisibility(ESlateVisibility::Hidden);
	}
}

void USTQuickSlotWidget::SetSheriffImage()
{
	ItemImage->SetBrushFromTexture(LockImage);
	ItemImage->SetVisibility(ESlateVisibility::Visible);
	ItemCountBox->SetVisibility(ESlateVisibility::Hidden);
	SelectBorder->SetVisibility(ESlateVisibility::Hidden);
}

void USTQuickSlotWidget::SetGhostImage()
{
	ItemImage->SetBrushFromTexture(LockImage);
	ItemImage->SetVisibility(ESlateVisibility::Visible);
	ItemCountBox->SetVisibility(ESlateVisibility::Hidden);
	SelectBorder->SetVisibility(ESlateVisibility::Hidden);
}
