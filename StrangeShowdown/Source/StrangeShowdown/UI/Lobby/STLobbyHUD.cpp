// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Lobby/STLobbyHUD.h"

void USTLobbyHUD::SetReady(const uint64 InPlayerID, const bool InReady)
{
	if (IsValid(LobbyStatusWidget))
	{
		LobbyStatusWidget->SetPlayerReady(InPlayerID, InReady);
	}

	if (IsValid(ReadyText))
	{
		ReadyText->SetVisibility(!InReady ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}
