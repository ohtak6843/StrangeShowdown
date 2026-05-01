// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Victory/STVictoryWidget.h"
#include "Components/TextBlock.h"

USTVictoryWidget::USTVictoryWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void USTVictoryWidget::SetWinnerPlayerName(const FString& PlayerName)
{
	WinnerPlayerNameText->SetText(FText::FromString(PlayerName));
}
