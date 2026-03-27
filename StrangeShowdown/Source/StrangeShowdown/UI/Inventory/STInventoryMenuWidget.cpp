// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/STInventoryMenuWidget.h"
#include "UI/Inventory/STInventoryWidget.h"

USTInventoryMenuWidget::USTInventoryMenuWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void USTInventoryMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Inventory = Cast<USTInventoryWidget>(GetWidgetFromName(TEXT("WidgetInventory")));

	APlayerController* PC = GetOwningPlayer();
	if (PC)
	{
		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetHideCursorDuringCapture(true);

		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = true;
	}
}

void USTInventoryMenuWidget::NativeDestruct()
{
	Super::NativeDestruct();
	
	APlayerController* PC = GetOwningPlayer();
	if (PC)
	{
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = false;
	}
}

void USTInventoryMenuWidget::UpdateInventory(const TArray<FSTItemSlot>& InItemSlots)
{
	if (Inventory)
	{
		Inventory->UpdateInventory(InItemSlots);
	}
}