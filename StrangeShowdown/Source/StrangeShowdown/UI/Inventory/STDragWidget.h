// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameData/STItemSlot.h"
#include "STDragWidget.generated.h"

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API USTDragWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	USTDragWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

public:
	FORCEINLINE void SetDragImage(class UImage* InItemImage) { DragItemImage = InItemImage; }

protected:
	UPROPERTY()
	TObjectPtr<class UImage> DragItemImage;
};
