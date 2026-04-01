// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/STInventoryMenuWidget.h"
#include "UI/Inventory/STInventoryWidget.h"
#include "Component/STInventoryComponent.h"

USTInventoryMenuWidget::USTInventoryMenuWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void USTInventoryMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Inventory = Cast<USTInventoryWidget>(GetWidgetFromName(TEXT("WidgetInventory")));

	// TODO: STHUDWidget에서 OpenInventoryMenu() 쪽에서 하는 걸로 변경하기
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

void USTInventoryMenuWidget::SetInventoryComponent(USTInventoryComponent* InInventoryComp)
{
	if (InInventoryComp)
	{
		SourceInventoryComp = InInventoryComp;
	}
}

void USTInventoryMenuWidget::UpdateInventory()
{
	if (Inventory && SourceInventoryComp.IsValid())
	{
		Inventory->UpdateInventory(SourceInventoryComp->Slots);
	}
}