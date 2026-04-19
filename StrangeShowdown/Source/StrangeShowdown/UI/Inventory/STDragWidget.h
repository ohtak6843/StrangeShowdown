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

public:
	FORCEINLINE void SetDragImage(class UTexture2D* InItemImage);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> DragItemImage;
};
