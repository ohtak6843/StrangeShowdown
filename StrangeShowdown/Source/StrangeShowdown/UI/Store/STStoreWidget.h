// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "STStoreWidget.generated.h"

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API USTStoreWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	USTStoreWidget(const FObjectInitializer& ObjectInitializer);

public:
	class UWrapBox* GetStoreSlotWrapBox() const { return StoreSlotWrapBox; }

	void SetupStore(int32 InStoreCount);
	void UpdateStore(const TArray<struct FStoreSlot>& InStoreSlots);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class USTStoreSlotWidget> StoreSlotClass;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UWrapBox> StoreSlotWrapBox;
};
