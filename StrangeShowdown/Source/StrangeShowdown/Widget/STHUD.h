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

public:
	USTMiniMapWidget* GetMiniMapWidget() const { return MiniMapWidget; }

};
