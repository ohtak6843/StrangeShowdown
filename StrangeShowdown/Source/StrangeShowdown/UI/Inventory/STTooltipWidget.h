// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameData/STItemSlot.h"
#include "STTooltipWidget.generated.h"

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API USTTooltipWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	USTTooltipWidget(const FObjectInitializer& ObjectInitializer);

public:
	void UpdateTooltip(const FSTItemSlot& ItemSlot);

protected:
	UPROPERTY()
	FSTItemSlot SlotData;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> ItemImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> ItemName;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> ItemDescription;
};
