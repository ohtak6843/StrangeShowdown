// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "STMiniMapWidget.generated.h"

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API USTMiniMapWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void UpdateItemIcon(class ASTPickupItem* Item, const FVector2D& MiniMapPos);
	void HideItemIcon(class ASTPickupItem* Item);

public:
	UPROPERTY(meta = (BindWidget))
	class UImage* PlayerTexture;
};
