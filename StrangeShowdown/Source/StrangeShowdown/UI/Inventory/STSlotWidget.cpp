// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/STSlotWidget.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Item/STItemDataAssetBase.h"

USTSlotWidget::USTSlotWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void USTSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ItemImage = Cast<UImage>(GetWidgetFromName(TEXT("ImageItem")));
	ensure(ItemImage);

	ItemCountBox = Cast<USizeBox>(GetWidgetFromName(TEXT("SizeBoxItemCount")));
	ensure(ItemCountBox);

	ItemCount = Cast<UTextBlock>(GetWidgetFromName(TEXT("TextItemCount")));
	ensure(ItemCount);
}

void USTSlotWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	OnSlotMouseEnter.ExecuteIfBound(SlotData);
}

FReply USTSlotWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply ReplyResult = Super::NativeOnMouseMove(InGeometry, InMouseEvent);

	OnSlotMouseMove.ExecuteIfBound(InMouseEvent);

	return ReplyResult;
}

void USTSlotWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	OnSlotMouseLeave.ExecuteIfBound();
}

void USTSlotWidget::UpdateSlot(const FSTItemSlot& ItemSlot)
{
	SlotData = ItemSlot;

	if (SlotData.ItemData)
	{
		ItemImage->SetBrushFromTexture(SlotData.ItemData->Icon);
		ItemCount->SetText(FText::AsNumber(SlotData.Count));
		ItemCountBox->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		ItemImage->SetVisibility(ESlateVisibility::Hidden);
		ItemCountBox->SetVisibility(ESlateVisibility::Hidden);
	}
}
