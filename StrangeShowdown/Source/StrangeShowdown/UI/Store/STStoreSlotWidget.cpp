// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Store/STStoreSlotWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Component/STStoreComponent.h"
#include "Item/STItemDataAssetBase.h"

USTStoreSlotWidget::USTStoreSlotWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void USTStoreSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ItemSlotButton)
	{
		ItemSlotButton->OnClicked.AddDynamic(this, &USTStoreSlotWidget::HandleItemSlotButtonClicked);
	}
}

void USTStoreSlotWidget::HandleItemSlotButtonClicked()
{
	OnItemSlotButtonClicked.Broadcast();
}

void USTStoreSlotWidget::UpdateSlot(const FStoreSlot& InStoreSlot)
{
	if (InStoreSlot.ItemData && InStoreSlot.bIsSold)
	{
		ItemImage->SetBrushFromTexture(InStoreSlot.ItemData->Icon);
		ItemNameText->SetText(InStoreSlot.ItemData->ItemName);
		GoldCostText->SetText(FText::AsNumber(InStoreSlot.ItemData->GoldCost));

		ItemImage->SetVisibility(ESlateVisibility::Visible);
		ItemNameText->SetVisibility(ESlateVisibility::Visible);
		GoldCostText->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		ItemImage->SetVisibility(ESlateVisibility::Hidden);
		ItemNameText->SetVisibility(ESlateVisibility::Hidden);
		GoldCostText->SetVisibility(ESlateVisibility::Hidden);
	}
}