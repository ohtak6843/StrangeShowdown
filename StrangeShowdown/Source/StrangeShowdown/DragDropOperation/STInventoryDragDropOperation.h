// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "STInventoryDragDropOperation.generated.h"

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API USTInventoryDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TObjectPtr<class USTInventoryComponent> SourceInventory;

	UPROPERTY()
	int32 SourceSlotIndex;
};
