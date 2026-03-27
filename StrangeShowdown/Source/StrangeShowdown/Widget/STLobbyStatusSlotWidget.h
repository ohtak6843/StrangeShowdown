// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "STLobbyStatusSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class USTLobbyStatusSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void Init(uint64 PlayerID);

	void Ready(bool bReady);

	UPROPERTY()
	uint64 PlayerID;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* NickNameText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* ReadyIcon;
};