// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/STHUDWidget.h"
#include "Components/WrapBox.h"
#include "UI/Stat/STStatWidget.h"
#include "UI/STQuickSlotWidget.h"
#include "Component/STQuickSlotComponent.h"
#include "UI/Inventory/STInventoryMenuWidget.h"
#include "Component/STInventoryComponent.h"
#include "Interface/STCharacterHUDInterface.h"
#include "Widget/STChatManagerWidget.h"

USTHUDWidget::USTHUDWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
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

void USTHUDWidget::UpdateStat()
{
	if (StatWidget)
	{
		StatWidget->UpdateStat();
	}
}

void USTHUDWidget::OpenInventoryMenu()
{
	if (nullptr == InventoryMenuWidget)
	{
		InventoryMenuWidget = CreateWidget<USTInventoryMenuWidget>(this, InventoryMenuClass);
		InventoryMenuWidget->AddToViewport();
		InventoryMenuWidget->UpdateInventory();
	}
	else
	{
		InventoryMenuWidget->RemoveFromParent();
		InventoryMenuWidget = nullptr;
	}
}

void USTHUDWidget::UpdateInventoryMenu()
{
	if (InventoryMenuWidget)
	{
		InventoryMenuWidget->UpdateInventory();
	}
}

USTQuickSlotWidget* USTHUDWidget::GetQuickSlotWidget(int32 Index)
{
	return Cast<USTQuickSlotWidget>(QuickSlotWrapBox->GetChildAt(Index));
}

void USTHUDWidget::SetQuickSlotComponent(USTQuickSlotComponent* InQuickSlotComp)
{
	if (InQuickSlotComp)
	{
		SourceQuickSlotComp = InQuickSlotComp;
	}
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

void USTHUDWidget::UpdateQuickSlots()
{
	if (SourceQuickSlotComp.IsValid())
	{
		int32 QuickSlotWidgetCount = QuickSlotWrapBox->GetChildrenCount();
		int32 QuickSlotItemCount = SourceQuickSlotComp->QuickSlots.Num();

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
				QuickSlotWidget->UpdateQuickSlot(SourceQuickSlotComp->QuickSlots[i], SourceQuickSlotComp->CurrentSelectQuickSlotIndex);
			}
		}
	}
}

void USTHUDWidget::OpenStoreMenu()
{
}

void USTHUDWidget::UpdateStoreMenu()
{
}

void USTHUDWidget::FocusChatManager()
{
	APlayerController* PlayerController = GetOwningPlayer();
	if (PlayerController)
	{
		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetHideCursorDuringCapture(true);

		PlayerController->SetInputMode(InputMode);
		PlayerController->bShowMouseCursor = true;
	}

	if (ChatManagerWidget)
	{
		ChatManagerWidget->SetChatInputFocus();
	}
}

void USTHUDWidget::UpdateChatManager()
{
}

void USTHUDWidget::UpdateMiniMap()
{
}

void USTHUDWidget::UpdateBigMap()
{
}
