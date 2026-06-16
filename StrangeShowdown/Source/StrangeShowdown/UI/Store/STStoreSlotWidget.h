// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Component/STStoreComponent.h"
#include "STStoreSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API USTStoreSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	USTStoreSlotWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

public:
	UFUNCTION()
	void HandleItemSlotButtonHovered();

	UFUNCTION()
	void HandleItemSlotButtonClicked();

	void UpdateSlot(const struct FStoreSlot& InStoreSlot);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FStoreSlot StoreSlot;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> ItemSlotButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> ItemImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> ItemNameText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> ItemDescriptionText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> GoldCostText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sound)
	TObjectPtr<class USoundBase> HoverSound;
};
