// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameData/STItemSlot.h"
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
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	void SetInventoryComponent(class USTInventoryComponent* InInventoryComp);

	void UpdateInventory();

protected:
	TWeakObjectPtr<class USTInventoryComponent> SourceInventoryComp;

	UPROPERTY()
	TObjectPtr<class USTInventoryWidget> Inventory;
};
