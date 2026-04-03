// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/STLobbyStatusWidget.h"
#include "Game/STGameState.h"
#include "GameFramework/PlayerState.h"

void USTLobbyStatusWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void USTLobbyStatusWidget::EnterPlayer(uint64 PlayerID, const FString& NickName)
{
	// 이미 존재하면 무시
	if (PlayerIDMap.Contains(PlayerID))
		return;

	// 방장 처리 (ID = 0 가정)
	if (PlayerID == 0)
	{
		PlayerIDMap.Add(PlayerID, 0);

		RoomOwnerNickName->SetText(FText::FromString(NickName));
		RoomOwnerNickName->SetVisibility(ESlateVisibility::Visible);

		return;
	}

	// 일반 플레이어
	if (NextSlotIndex >= MaxPlayerCount)
		return;

	PlayerIDMap.Add(PlayerID, NextSlotIndex);

	switch (NextSlotIndex)
	{
	case 1:
		Player1NickName->SetText(FText::FromString(NickName));
		Player1NickName->SetVisibility(ESlateVisibility::Visible);
		break;
	case 2:
		Player2NickName->SetText(FText::FromString(NickName));
		Player2NickName->SetVisibility(ESlateVisibility::Visible);
		break;
	case 3:
		Player3NickName->SetText(FText::FromString(NickName));
		Player3NickName->SetVisibility(ESlateVisibility::Visible);
		break;
	case 4:
		Player4NickName->SetText(FText::FromString(NickName));
		Player4NickName->SetVisibility(ESlateVisibility::Visible);
		break;
	}

	NextSlotIndex++;
}

void USTLobbyStatusWidget::SetPlayerReady(uint64 PlayerID, bool bReady)
{
	int32* SlotNum = PlayerIDMap.Find(PlayerID);

	if (!SlotNum)
		return;

	// 방장 제외
	if (*SlotNum == 0)
		return;

	switch (*SlotNum)
	{
	case 1:
		Player1ReadyIcon->SetVisibility(bReady ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
		break;
	case 2:
		Player2ReadyIcon->SetVisibility(bReady ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
		break;
	case 3:
		Player3ReadyIcon->SetVisibility(bReady ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
		break;
	case 4:
		Player4ReadyIcon->SetVisibility(bReady ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
		break;
	}
}