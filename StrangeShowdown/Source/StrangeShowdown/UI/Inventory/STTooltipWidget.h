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

protected:
	virtual void NativeConstruct() override;

public:
	void UpdateTooltip(const FSTItemSlot& ItemSlot);

protected:
	UPROPERTY()
	FSTItemSlot SlotData;

	UPROPERTY()
	TObjectPtr<class UImage> ItemImage;

	UPROPERTY()
	TObjectPtr<class UTextBlock> ItemName;

	UPROPERTY()
	TObjectPtr<class UTextBlock> ItemDescription;
};
