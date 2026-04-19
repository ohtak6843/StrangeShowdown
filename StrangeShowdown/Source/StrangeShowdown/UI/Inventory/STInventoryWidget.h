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

public:
	void SetupInventory(int32 InventorySlotCount);

	void UpdateInventory(const TArray<FSTItemSlot>& InItemSlots);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Widget)
	TSubclassOf<class USTSlotWidget> SlotWidgetClass;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UWrapBox> SlotWrapBox;
};
