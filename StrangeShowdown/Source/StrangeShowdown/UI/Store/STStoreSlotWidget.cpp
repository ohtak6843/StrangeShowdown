// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Store/STStoreSlotWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Component/STStoreComponent.h"
#include "Item/STItemDataAssetBase.h"
#include "Kismet/GameplayStatics.h"
#include "Interface/STCharacterHUDInterface.h"

USTStoreSlotWidget::USTStoreSlotWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void USTStoreSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ItemSlotButton)
	{
		ItemSlotButton->OnHovered.AddDynamic(this, &USTStoreSlotWidget::HandleItemSlotButtonHovered);
		ItemSlotButton->OnClicked.AddDynamic(this, &USTStoreSlotWidget::HandleItemSlotButtonClicked);
	}
}

void USTStoreSlotWidget::HandleItemSlotButtonHovered()
{
	if (HoverSound)
	{
		float VolumeMultiplier = 0.8f;
		UGameplayStatics::PlaySound2D(this, HoverSound, VolumeMultiplier);
	}
}

void USTStoreSlotWidget::HandleItemSlotButtonClicked()
{
	if(StoreSlot.ItemData && false == StoreSlot.bIsSold)
	{
		ISTCharacterHUDInterface* HUDInterface = Cast<ISTCharacterHUDInterface>(GetOwningPlayerPawn());
		if (HUDInterface)
		{
			HUDInterface->HandleStoreSlotClicked(StoreSlot);
		}
	}
}

void USTStoreSlotWidget::UpdateSlot(const FStoreSlot& InStoreSlot)
{
	StoreSlot = InStoreSlot;

	if (StoreSlot.ItemData && !StoreSlot.bIsSold)
	{
		ItemImage->SetBrushFromTexture(StoreSlot.ItemData->Icon);
		ItemNameText->SetText(StoreSlot.ItemData->ItemName);
		ItemDescriptionText->SetText(StoreSlot.ItemData->Description);
		GoldCostText->SetText(FText::AsNumber(StoreSlot.ItemData->GoldCost));

		ItemImage->SetVisibility(ESlateVisibility::Visible);
		ItemNameText->SetVisibility(ESlateVisibility::Visible);
		ItemDescriptionText->SetVisibility(ESlateVisibility::Visible);
		GoldCostText->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		ItemImage->SetVisibility(ESlateVisibility::Hidden);
		ItemNameText->SetVisibility(ESlateVisibility::Hidden);
		ItemDescriptionText->SetVisibility(ESlateVisibility::Hidden);
		GoldCostText->SetVisibility(ESlateVisibility::Hidden);
	}
}