// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget/STChatManagerWidget.h"
#include "Widget/STStatWidget.h"
#include "Widget/STMissionWidget.h"
#include "Widget/STMiniMapWidget.h"
#include "STHUD.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EHUDWidgetType : uint8
{
	Player,
	Ghost,
	Sheriff
};

UCLASS()
class STRANGESHOWDOWN_API USTHUD : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USTChatManagerWidget* ChatManagerWidget;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USTStatWidget* StatWidget;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USTMissionWidget* MissionWidget;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USTMiniMapWidget* MiniMapWidget;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USTMiniMapWidget* BigMapWidget;

public:
	USTMiniMapWidget* GetMiniMapWidget() const { return MiniMapWidget; }
	USTMiniMapWidget* GetBigMapWidget() const { return BigMapWidget; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
	EHUDWidgetType HUDWidgetType = EHUDWidgetType::Player;
};
