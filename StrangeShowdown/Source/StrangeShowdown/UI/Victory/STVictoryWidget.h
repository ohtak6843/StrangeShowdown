// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "STVictoryWidget.generated.h"

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API USTVictoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	USTVictoryWidget(const FObjectInitializer& ObjectInitializer);

public:
	void SetWinnerPlayerName(const FString& PlayerName);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> WinnerText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> WinnerPlayerNameText;
};
