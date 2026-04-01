// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/STTooltipWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Item/STItemDataAssetBase.h"

USTTooltipWidget::USTTooltipWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void USTTooltipWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ItemImage = Cast<UImage>(GetWidgetFromName(TEXT("ImageItem")));
	ensure(ItemImage);
	ItemName = Cast<UTextBlock>(GetWidgetFromName(TEXT("TextItemName")));
	ensure(ItemName);
	ItemDescription = Cast<UTextBlock>(GetWidgetFromName(TEXT("TextItemDescription")));
	ensure(ItemDescription);
}

void USTTooltipWidget::UpdateTooltip(const FSTItemSlot& ItemSlot)
{
	SlotData = ItemSlot;

	if (SlotData.ItemData)
	{
		ItemImage->SetBrushFromTexture(SlotData.ItemData->Icon);
		ItemName->SetText(SlotData.ItemData->ItemName);
		ItemDescription->SetText(SlotData.ItemData->Description);
	}
}