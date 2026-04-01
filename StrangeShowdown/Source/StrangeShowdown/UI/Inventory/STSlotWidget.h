// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameData/STItemSlot.h"
#include "STSlotWidget.generated.h"

DECLARE_DELEGATE_OneParam(FOnSlotMouseEnter, const FSTItemSlot& /*ItemSlot*/)
DECLARE_DELEGATE_OneParam(FOnSlotMouseMove, const FPointerEvent& /*InMouseEvent*/)
DECLARE_DELEGATE(FOnSlotMouseLeave);

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API USTSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	USTSlotWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

public:
	void UpdateSlot(const FSTItemSlot& ItemSlot);

	FOnSlotMouseEnter OnSlotMouseEnter;
	FOnSlotMouseMove OnSlotMouseMove;
	FOnSlotMouseLeave OnSlotMouseLeave;

protected:
	UPROPERTY()
	FSTItemSlot SlotData;

	UPROPERTY()
	TObjectPtr<class UImage> ItemImage;

	UPROPERTY()
	TObjectPtr<class USizeBox> ItemCountBox;

	UPROPERTY()
	TObjectPtr<class UTextBlock> ItemCount;
};
