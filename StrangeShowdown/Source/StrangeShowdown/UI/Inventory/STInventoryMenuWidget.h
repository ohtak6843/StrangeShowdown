// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameData/STSlotData.h"
#include "STInventoryMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API USTInventoryMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	USTInventoryMenuWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeDestruct() override;

public:
	void UpdateInventory(const TArray<FSTItemSlot>& InItemSlots);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class USTInventoryWidget> InventoryWidget;
};
