// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameData/STItemSlot.h"
#include "STInventoryWidget.generated.h"

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API USTInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	USTInventoryWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

public:
	void SetupInventory(int32 InventorySlotCount);

	void UpdateInventory(const TArray<FSTItemSlot>& InItemSlots);

protected:
	void HandleSlotMouseEnter(const FSTItemSlot& ItemSlot);
	void HandleSlotMouseMove(const FPointerEvent& InMouseEvent);
	void HandleSlotMouseLeave();

protected:
	UPROPERTY()
	TSubclassOf<class USTSlotWidget> SlotWidgetClass;

	UPROPERTY()
	TObjectPtr<class UWrapBox> SlotWrapBox;

	UPROPERTY()
	TObjectPtr<class USTTooltipWidget> Tooltip;
};
