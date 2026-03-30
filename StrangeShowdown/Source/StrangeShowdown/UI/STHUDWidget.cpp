// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/STHUDWidget.h"
#include "Components/WrapBox.h"
#include "UI/STQuickSlotWidget.h"
#include "Component/STQuickSlotComponent.h"
#include "Interface/STCharacterHUDInterface.h"
#include "UI/Inventory/STInventoryMenuWidget.h"
#include "Component/STInventoryComponent.h"

USTHUDWidget::USTHUDWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<USTInventoryMenuWidget> InventoryMenuWidgetRef(TEXT("/Game/StrangeShowdown/UITest/Inventory/WBP_InventoryMenu.WBP_InventoryMenu_C"));
	if (InventoryMenuWidgetRef.Class)
	{
		InventoryMenuClass = InventoryMenuWidgetRef.Class;
	}

	static ConstructorHelpers::FClassFinder<USTQuickSlotWidget> QuickSlotWidgetRef(TEXT("/Game/StrangeShowdown/UITest/WBP_QuickSlot.WBP_QuickSlot_C"));
	if (QuickSlotWidgetRef.Class)
	{
		QuickSlotWidgetClass = QuickSlotWidgetRef.Class;
	}
}

void USTHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ISTCharacterHUDInterface* HUDPawn = Cast<ISTCharacterHUDInterface>(GetOwningPlayerPawn());
	if (HUDPawn)
	{
		HUDPawn->SetupHUDWidget(this);
	}

	// TODO: 나중에 위젯 타입 어떻게 할지 고민하기
	if (StatWidget)
	{
		StatWidget->SetWidgetType(HUDWidgetType);
	}
}

void USTHUDWidget::UpdateMission()
{
}

void USTHUDWidget::UpdateMiniMap()
{
}

void USTHUDWidget::UpdateWorldMap()
{
}

void USTHUDWidget::UpdateStat()
{
	if (StatWidget)
	{
		StatWidget->UpdateStat();
	}
}

void USTHUDWidget::ShowInventoryMenu(const TArray<FSTItemSlot>& InItemSlots)
{
	if (nullptr == InventoryMenuWidget)
	{
		InventoryMenuWidget = CreateWidget<USTInventoryMenuWidget>(this, InventoryMenuClass);
		InventoryMenuWidget->AddToViewport();
		InventoryMenuWidget->UpdateInventory(InItemSlots);
	}
	else
	{
		InventoryMenuWidget->RemoveFromParent();
		InventoryMenuWidget = nullptr;
	}
}

void USTHUDWidget::UpdateInventoryMenu(const TArray<FSTItemSlot>& InItemSlots)
{
	if (InventoryMenuWidget)
	{
		InventoryMenuWidget->UpdateInventory(InItemSlots);
	}
}

USTQuickSlotWidget* USTHUDWidget::GetQuickSlotWidget(int32 Index)
{
	return Cast<USTQuickSlotWidget>(QuickSlotWrapBox->GetChildAt(Index));
}

void USTHUDWidget::SetupQuickSlots(int32 QuickSlotCount)
{
	for (int32 i = 0; i < QuickSlotCount; i++)
	{
		USTQuickSlotWidget* QuickSlotWidget = CreateWidget<USTQuickSlotWidget>(this, QuickSlotWidgetClass);
		if (QuickSlotWidget)
		{
			QuickSlotWidget->SetPadding(FMargin(13.0f, 10.0f, 25.0f, 0.0f));
			QuickSlotWidget->SetSlotIndex(i);
			QuickSlotWrapBox->AddChild(QuickSlotWidget);
		}
	}
}

void USTHUDWidget::UpdateQuickSlots(const TArray<FSTItemSlot>& InItemSlots, int32 CurrentSelectedIndex)
{
	int32 QuickSlotWidgetCount = QuickSlotWrapBox->GetChildrenCount();
	int32 QuickSlotItemCount = InItemSlots.Num();

	// QuickSlot 개수와 QuickSlotComponent의 Item 개수가 다르면 새로 세팅
	if (QuickSlotWidgetCount != QuickSlotItemCount)
	{
		QuickSlotWrapBox->ClearChildren();
		SetupQuickSlots(QuickSlotItemCount);
	}

	for (int i = 0; i < QuickSlotWrapBox->GetChildrenCount(); i++)
	{
		USTQuickSlotWidget* QuickSlotWidget = GetQuickSlotWidget(i);
		if (QuickSlotWidget)
		{
			QuickSlotWidget->UpdateQuickSlot(InItemSlots[i], CurrentSelectedIndex);
		}
	}
}