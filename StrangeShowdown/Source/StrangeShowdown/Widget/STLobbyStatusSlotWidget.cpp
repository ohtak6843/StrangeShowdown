// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/STLobbyStatusSlotWidget.h"
#include "GameFramework/PlayerState.h"

void USTLobbyStatusSlotWidget::Init(uint64 NewPlayerID)
{
	PlayerID = NewPlayerID;

	NickNameText->SetText(FText::FromString(TEXT("Player")));
}

void USTLobbyStatusSlotWidget::Ready(bool bReady)
{
	if (ReadyIcon)
	{
		ReadyIcon->SetVisibility(bReady ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}