// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "STMissionWidget.generated.h"

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API USTMissionWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* MissionText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* MissionTitle;

public:
	UFUNCTION(BlueprintCallable, Category = "Mission")
	void SetMission(const FText& NewTitle, const FText& NewMission);
};
