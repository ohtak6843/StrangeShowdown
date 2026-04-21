// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/STHUDWidget.h"
#include "Components/WrapBox.h"
#include "UI/Stat/STStatWidget.h"
#include "UI/QuickSlot/STQuickSlotWidget.h"
#include "Component/STQuickSlotComponent.h"
#include "UI/Inventory/STInventoryMenuWidget.h"
#include "Component/STInventoryComponent.h"
#include "UI/Store/STStoreMenuWidget.h"
#include "Component/STStoreComponent.h"
#include "Interface/STCharacterHUDInterface.h"
#include "Widget/STChatManagerWidget.h"
#include "Widget/STMiniMapWidget.h"
#include "UI/BountyPoster/STBountyPosterMenuWidget.h"
#include "UI/Timer/STTimerWidget.h"

DEFINE_LOG_CATEGORY(LogSTHUDWidget);

USTHUDWidget::USTHUDWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	HUDWidgetType = EHUDWidgetType::Player;
}

void USTHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ISTCharacterHUDInterface* HUDPawn = Cast<ISTCharacterHUDInterface>(GetOwningPlayerPawn());
	if (HUDPawn)
	{
		HUDPawn->SetupHUDWidget(this);
	}

	SetWidgetType(HUDWidgetType);
}

void USTHUDWidget::SetWidgetType(EHUDWidgetType InWidgetType)
{
	HUDWidgetType = InWidgetType;

	if (StatWidget)
	{
		switch (HUDWidgetType)
		{
		case EHUDWidgetType::Player:
			StatWidget->SetPlayerWidget();
			break;
		case EHUDWidgetType::Ghost:
			StatWidget->SetGhostWidget();
			for (int i = 0; i < QuickSlotWrapBox->GetChildrenCount(); i++)
			{
				USTQuickSlotWidget* QuickSlotWidget = GetQuickSlotWidget(i);
				if (QuickSlotWidget)
				{
					QuickSlotWidget->SetGhostImage();
				}
			}
			break;
		case EHUDWidgetType::Sheriff:
			StatWidget->SetSheriffWidget();
			for (int i = 0; i < QuickSlotWrapBox->GetChildrenCount(); i++)
			{
				USTQuickSlotWidget* QuickSlotWidget = GetQuickSlotWidget(i);
				if (QuickSlotWidget)
				{
					QuickSlotWidget->SetSheriffImage();
				}
			}
			break;
		}
	}
}

void USTHUDWidget::UpdateStat()
{
	if (StatWidget)
	{
		StatWidget->UpdateStat();
	}
}

void USTHUDWidget::SetInventoryComponent(USTInventoryComponent* InInventoryComp)
{
	if (InInventoryComp)
	{
		SourceInventoryComp = InInventoryComp;
	}
}

bool USTHUDWidget::OpenInventoryMenu()
{
	if (nullptr == InventoryMenuWidget)
	{
		InventoryMenuWidget = CreateWidget<USTInventoryMenuWidget>(this, InventoryMenuClass);
		InventoryMenuWidget->AddToViewport();
		InventoryMenuWidget->UpdateInventory(SourceInventoryComp.Get()->Slots);

		APlayerController* PC = GetOwningPlayer();
		if (PC)
		{
			FInputModeGameAndUI InputMode;
			InputMode.SetWidgetToFocus(InventoryMenuWidget->TakeWidget());
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			InputMode.SetHideCursorDuringCapture(true);

			PC->SetInputMode(InputMode);
			PC->bShowMouseCursor = true;

			return true;
		}

		UE_LOG(LogSTHUDWidget, Log, TEXT("Failed to open Inventory Menu: PlayerController is null"));
		return false;
	}

	UE_LOG(LogSTHUDWidget, Log, TEXT("Failed to open Inventory Menu: InventoryMenuWidget is already open"));
	return false;
}

bool USTHUDWidget::CloseInventoryMenu()
{
	if(InventoryMenuWidget)
	{
		InventoryMenuWidget->RemoveFromParent();
		InventoryMenuWidget = nullptr;

		APlayerController* PC = GetOwningPlayer();
		if (PC)
		{
			FInputModeGameOnly GameOnlyInputMode;
			PC->SetInputMode(GameOnlyInputMode);

			return true;
		}

		UE_LOG(LogSTHUDWidget, Log, TEXT("Failed to close Inventory Menu: PlayerController is null"));
		return false;
	}

	UE_LOG(LogSTHUDWidget, Log, TEXT("Failed to close Inventory Menu: InventoryMenuWidget is not open"));
	return false;
}

void USTHUDWidget::UpdateInventoryMenu()
{
	if (InventoryMenuWidget)
	{
		InventoryMenuWidget->UpdateInventory(SourceInventoryComp.Get()->Slots);
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
			if(SourceQuickSlotComp.IsValid())
			{
				QuickSlotWidget->OnQuickSlotWidgetDrop.BindUObject(SourceQuickSlotComp.Get(), &USTQuickSlotComponent::AddItem);
			}
			QuickSlotWrapBox->AddChild(QuickSlotWidget);
		}
	}
}

void USTHUDWidget::UpdateQuickSlots()
{
	if (SourceQuickSlotComp.IsValid())
	{
		int32 QuickSlotWidgetCount = QuickSlotWrapBox->GetChildrenCount();
		int32 QuickSlotItemCount = SourceQuickSlotComp.Get()->QuickSlots.Num();

		// QuickSlot 개수와 QuickSlotComponent의 Item 개수가 다르면 새로 세팅
		if (QuickSlotWidgetCount != QuickSlotItemCount)
		{
			QuickSlotWrapBox->ClearChildren();
			SetupQuickSlots(QuickSlotItemCount);
		}

		if (EHUDWidgetType::Player == HUDWidgetType)
		{
			for (int i = 0; i < QuickSlotWrapBox->GetChildrenCount(); i++)
			{
				USTQuickSlotWidget* QuickSlotWidget = GetQuickSlotWidget(i);
				if (QuickSlotWidget)
				{
					QuickSlotWidget->UpdateQuickSlot(SourceQuickSlotComp.Get()->QuickSlots[i], SourceQuickSlotComp.Get()->CurrentSelectQuickSlotIndex);
				}
			}
		}
	}
}

void USTHUDWidget::SetStoreComponent(USTStoreComponent* InStoreComp)
{
	if (InStoreComp)
	{
		SourceStoreComp = InStoreComp;
	}
}

bool USTHUDWidget::OpenStoreMenu()
{
	if(SourceStoreComp.IsValid())
	{
		if (!StoreMenuWidget)
		{
			StoreMenuWidget = CreateWidget<USTStoreMenuWidget>(this, StoreMenuClass);
			if (StoreMenuWidget)
			{
				StoreMenuWidget->OnExitButtonClicked.BindUObject(this, &USTHUDWidget::CloseStoreMenu);
				StoreMenuWidget->AddToViewport();
				StoreMenuWidget->UpdateStoreMenu(SourceStoreComp.Get()->Slots);
				APlayerController* PC = GetOwningPlayer();
				if (PC)
				{
					FInputModeGameAndUI InputMode;
					InputMode.SetWidgetToFocus(StoreMenuWidget->TakeWidget());
					InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
					InputMode.SetHideCursorDuringCapture(true);
					PC->SetInputMode(InputMode);
					PC->bShowMouseCursor = true;
					PC->SetIgnoreMoveInput(true);
					return true;
				}
				UE_LOG(LogSTHUDWidget, Log, TEXT("Failed to open Store Menu: PlayerController is null"));
				return false;
			}
			UE_LOG(LogSTHUDWidget, Log, TEXT("Failed to open Store Menu: StoreMenuWidget could not be created"));
			return false;
		}
		UE_LOG(LogSTHUDWidget, Log, TEXT("Failed to open Store Menu: StoreMenuWidget is already open"));
		return false;
	}

	return true;
}

bool USTHUDWidget::CloseStoreMenu()
{
	if (StoreMenuWidget)
	{
		StoreMenuWidget->RemoveFromParent();
		StoreMenuWidget = nullptr;
		APlayerController* PC = GetOwningPlayer();
		if (PC)
		{
			FInputModeGameOnly GameOnlyInputMode;
			PC->SetInputMode(GameOnlyInputMode);
			PC->bShowMouseCursor = false;
			PC->SetIgnoreMoveInput(false);
			return true;
		}
		UE_LOG(LogSTHUDWidget, Log, TEXT("Failed to close Store Menu: PlayerController is null"));
		return false;
	}

	return true;
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

bool USTHUDWidget::OpenBigMap()
{
	if (BigMapWidget)
	{
		// TODO: RemoveAllUI 할건가?
		BigMapWidget->SetVisibility(ESlateVisibility::Visible);
		return true;
	}

	return false;
}

bool USTHUDWidget::CloseBigMap()
{
	if (BigMapWidget)
	{
		BigMapWidget->SetVisibility(ESlateVisibility::Hidden);
		return true;
	}

	return false;
}

void USTHUDWidget::UpdateMiniMap()
{
}

void USTHUDWidget::UpdateBigMap()
{
}

bool USTHUDWidget::OpenBountyPoster()
{
	if (!BountyPosterMenuWidget)
	{
		BountyPosterMenuWidget = CreateWidget<USTBountyPosterMenuWidget>(this, BountyPosterMenuClass);
		if (BountyPosterMenuWidget)
		{
			BountyPosterMenuWidget->AddToViewport();
			BountyPosterMenuWidget->UpdateBountyPosterMenu();

			return true;
		}
	}

	UE_LOG(LogSTHUDWidget, Log, TEXT("Failed to open Bounty Poster: BountyPosterMenuWidget is already open"));
	return false;
}

bool USTHUDWidget::CloseBountyPoster()
{
	if (BountyPosterMenuWidget)
	{
		BountyPosterMenuWidget->RemoveFromParent();
		BountyPosterMenuWidget = nullptr;

		return true;
	}

	UE_LOG(LogSTHUDWidget, Log, TEXT("Failed to close Bounty Poster: BountyPosterMenuWidget is not open"));
	return false;
}

void USTHUDWidget::UpdateBountyPoster()
{
}