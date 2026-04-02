// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/STUserWidget.h"
#include "GameData/STItemSlot.h"
#include "STQuickSlotWidget.generated.h"

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnQuickSlotWidgetDrop, const FSTItemSlot& /*ItemSlot*/, int32 /*InventoryIndex*/, int32 /*QuickSlotIndex*/);

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API USTQuickSlotWidget : public USTUserWidget
{
	GENERATED_BODY()

public:
	USTQuickSlotWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

public:
	void UpdateQuickSlot(const FSTItemSlot& ItemSlot, int32 CurrentSelectedIndex);

	void SetSlotIndex(int32 NewSlotIndex) { SlotIndex = NewSlotIndex; }

	void SetSheriffImage();
	void SetGhostImage();

	FOnQuickSlotWidgetDrop OnQuickSlotWidgetDrop;

protected:

	UPROPERTY()
	int32 SlotIndex;

	UPROPERTY()
	FSTItemSlot SlotData;

	UPROPERTY()
	TObjectPtr<class UBorder> SelectBorder;

	UPROPERTY()
	TObjectPtr<class UImage> ItemImage;

	UPROPERTY()
	TObjectPtr<class USizeBox> ItemCountBox;

	UPROPERTY()
	TObjectPtr<class UTextBlock> ItemCount;

	UPROPERTY()
	TObjectPtr<class UTextBlock> SlotIndexText;

	UPROPERTY()
	TObjectPtr<class UTexture2D> LockImage;
};
