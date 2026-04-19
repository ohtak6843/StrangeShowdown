// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameData/STItemSlot.h"
#include "STSlotWidget.generated.h"

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
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

public:
	void UpdateSlot(const FSTItemSlot& ItemSlot, int32 Index);

protected:
	UPROPERTY()
	FSTItemSlot SlotData;

	UPROPERTY()
	int32 SlotIndex;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> ItemImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class USizeBox> ItemCountBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> ItemCount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Widget)
	TSubclassOf<class USTTooltipWidget> TooltipWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Widget)
	TObjectPtr<class USTTooltipWidget> Tooltip;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Widget)
	TSubclassOf<class USTDragWidget> DragWidgetClass;
};
