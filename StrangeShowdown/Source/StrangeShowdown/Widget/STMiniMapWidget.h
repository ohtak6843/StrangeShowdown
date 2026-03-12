// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget/STMiniMapItemIconWidget.h"
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

	// BigMap¿ë ÇÔ¼ö
	void UpdatePlayerIcon(const FVector2D& MiniMapPos);

	void SetIsRotationAble(bool RotationAble) { IsRotationAble = RotationAble; }

public:
	UPROPERTY(meta = (BindWidget))
	class UImage* PlayerTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MiniMap")
	TMap<ASTPickupItem*, UUserWidget*> ItemIconMap;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MiniMap")
	TSubclassOf<USTMiniMapItemIconWidget> ItemIconClass;

	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* ItemLayer;

private:
	bool IsRotationAble = true;
};
