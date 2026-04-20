// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Store/STStoreWidget.h"
#include "Components/WrapBox.h"
#include "UI/Store/STStoreSlotWidget.h"
#include "Component/STStoreComponent.h"

USTStoreWidget::USTStoreWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void USTStoreWidget::SetupStore(int32 InStoreCount)
{
	for (int i = 0; i < InStoreCount; i++)
	{
		USTStoreSlotWidget* StoreSlotWidget = CreateWidget<USTStoreSlotWidget>(this, StoreSlotClass);
		if (StoreSlotWidget)
		{
			StoreSlotWrapBox->AddChild(StoreSlotWidget);
		}
	}
}

void USTStoreWidget::UpdateStore(const TArray<FStoreSlot>& InStoreSlots)
{
	int32 StoreSlotWidgetCount = StoreSlotWrapBox->GetChildrenCount();
	int32 StoreItemCount = InStoreSlots.Num();
	if(StoreSlotWidgetCount != StoreItemCount)
	{
		StoreSlotWrapBox->ClearChildren();
		SetupStore(StoreItemCount);
	}

	for(int i=0; i<StoreSlotWrapBox->GetChildrenCount(); i++)
	{
		USTStoreSlotWidget* StoreSlotWidget = Cast<USTStoreSlotWidget>(StoreSlotWrapBox->GetChildAt(i));
		if (StoreSlotWidget)
		{
			StoreSlotWidget->UpdateSlot(InStoreSlots[i]);
		}
	}
}
