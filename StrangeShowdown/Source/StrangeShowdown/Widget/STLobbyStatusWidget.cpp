// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/STLobbyStatusWidget.h"
#include "Game/STGameState.h"
#include "GameFramework/PlayerState.h"

void USTLobbyStatusWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void USTLobbyStatusWidget::EnterPlayer(uint64 PlayerID, const FString& NickName, bool bReady)
{
	// 이미 존재하면 무시
	if (PlayerMap.Contains(PlayerID))
		return;

	// 방장 처리 (ID = 0 가정)
	// todo cham:
	if (PlayerID == 0)
	{
		PlayerMap.Add(PlayerID, FPlayerLobbyData{ PlayerID, NickName, false, true });

		RoomOwnerNickName->SetText(FText::FromString(NickName));
		RoomOwnerNickName->SetVisibility(ESlateVisibility::Visible);

		return;
	}

	// 일반 플레이어
	if (NextSlotIndex >= MaxPlayerCount)
		return;

	PlayerMap.Add(PlayerID, FPlayerLobbyData{ PlayerID, NickName, bReady, false });

	switch (NextSlotIndex)
	{
	case 1:
		Player1NickName->SetText(FText::FromString(NickName));
		Player1NickName->SetVisibility(ESlateVisibility::Visible);
		Player1ReadyIcon->SetVisibility(bReady ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
		break;
	case 2:
		Player2NickName->SetText(FText::FromString(NickName));
		Player2NickName->SetVisibility(ESlateVisibility::Visible);
		Player2ReadyIcon->SetVisibility(bReady ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
		break;
	case 3:
		Player3NickName->SetText(FText::FromString(NickName));
		Player3NickName->SetVisibility(ESlateVisibility::Visible);
		Player3ReadyIcon->SetVisibility(bReady ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
		break;
	case 4:
		Player4NickName->SetText(FText::FromString(NickName));
		Player4NickName->SetVisibility(ESlateVisibility::Visible);
		Player4ReadyIcon->SetVisibility(bReady ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
		break;
	}

	NextSlotIndex++;
}

void USTLobbyStatusWidget::LeavePlayer(uint64 PlayerID)
{
	if (!PlayerMap.Contains(PlayerID))
		return;

	PlayerMap.Remove(PlayerID);

	RebuildSlots();
}

void USTLobbyStatusWidget::RebuildSlots()
{
	// UI 전체 초기화
	RoomOwnerNickName->SetVisibility(ESlateVisibility::Hidden);

	Player1NickName->SetVisibility(ESlateVisibility::Hidden);
	Player2NickName->SetVisibility(ESlateVisibility::Hidden);
	Player3NickName->SetVisibility(ESlateVisibility::Hidden);
	Player4NickName->SetVisibility(ESlateVisibility::Hidden);

	Player1ReadyIcon->SetVisibility(ESlateVisibility::Hidden);
	Player2ReadyIcon->SetVisibility(ESlateVisibility::Hidden);
	Player3ReadyIcon->SetVisibility(ESlateVisibility::Hidden);
	Player4ReadyIcon->SetVisibility(ESlateVisibility::Hidden);

	NextSlotIndex = 1;

	// 방장 먼저 처리
	for (auto& Elem : PlayerMap)
	{
		const FPlayerLobbyData& Data = Elem.Value;

		if (Data.bIsHost)
		{
			RoomOwnerNickName->SetText(FText::FromString(Data.NickName));
			RoomOwnerNickName->SetVisibility(ESlateVisibility::Visible);
			break;
		}
	}

	// 일반 플레이어 정렬
	TArray<FPlayerLobbyData> Players;

	for (auto& Elem : PlayerMap)
	{
		const FPlayerLobbyData& Data = Elem.Value;

		if (!Data.bIsHost)
		{
			Players.Add(Data);
		}
	}

	// 정렬 기준
	Players.Sort([](const FPlayerLobbyData& A, const FPlayerLobbyData& B)
		{
			return A.PlayerID < B.PlayerID;
		});

	// 슬롯 배치
	for (const FPlayerLobbyData& Data : Players)
	{
		if (NextSlotIndex >= MaxPlayerCount)
			break;

		switch (NextSlotIndex)
		{
		case 1:
			Player1NickName->SetText(FText::FromString(Data.NickName));
			Player1NickName->SetVisibility(ESlateVisibility::Visible);
			Player1ReadyIcon->SetVisibility(Data.bReady ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
			break;

		case 2:
			Player2NickName->SetText(FText::FromString(Data.NickName));
			Player2NickName->SetVisibility(ESlateVisibility::Visible);
			Player2ReadyIcon->SetVisibility(Data.bReady ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
			break;

		case 3:
			Player3NickName->SetText(FText::FromString(Data.NickName));
			Player3NickName->SetVisibility(ESlateVisibility::Visible);
			Player3ReadyIcon->SetVisibility(Data.bReady ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
			break;

		case 4:
			Player4NickName->SetText(FText::FromString(Data.NickName));
			Player4NickName->SetVisibility(ESlateVisibility::Visible);
			Player4ReadyIcon->SetVisibility(Data.bReady ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
			break;
		}

		NextSlotIndex++;
	}
}

uint64 USTLobbyStatusWidget::GetSlotIndex(uint64 PlayerID)
{
	uint64 Index = 1;

	for (auto& Elem : PlayerMap)
	{
		const FPlayerLobbyData& Data = Elem.Value;

		if (Data.bIsHost)
			continue;

		if (Data.PlayerID == PlayerID)
			return Index;

		Index++;
	}

	return -1;
}

void USTLobbyStatusWidget::SetPlayerReady(uint64 PlayerID, bool bReady)
{
	FPlayerLobbyData* Data = PlayerMap.Find(PlayerID);
	if (!Data)
		return;

	Data->bReady = bReady;

	int32 SlotIndex = GetSlotIndex(PlayerID);

	switch (SlotIndex)
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