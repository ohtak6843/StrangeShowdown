// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/STLobbyStatusSlotWidget.h"
#include "GameFramework/PlayerState.h"
#include "Game/STGameInstance.h"

void USTLobbyStatusSlotWidget::Init(uint64 NewPlayerID, const FString& NickName)
{
	PlayerID = NewPlayerID;

	NickNameText->SetText(FText::FromString(NickName));
}

void USTLobbyStatusSlotWidget::Ready(bool bReady)
{
	if (ReadyIcon)
	{
		ReadyIcon->SetVisibility(bReady ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}