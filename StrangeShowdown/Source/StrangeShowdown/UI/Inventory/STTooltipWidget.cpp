// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/STTooltipWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Item/STItemDataAssetBase.h"

USTTooltipWidget::USTTooltipWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
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