// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/STLobbyStatusWidget.h"
#include "Game/STGameState.h"
#include "GameFramework/PlayerState.h"

#include "Game/STGameInstance.h"
#include "Manager/STDataManager.h"

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
	uint64 HostID{ 0 };
	
#if NETWORK_ENABLED

	// 데이터 메니저에서 호스트 ID를 가져오기
	auto* GameInstance{ Cast<USTGameInstance>(GetWorld()->GetGameInstance()) };
	auto* DataManager{ GameInstance ? GameInstance->GetDataManager() : nullptr };
	if (DataManager)
	{
		HostID = DataManager->GetHostID();
	}

#endif // NETWORK_ENABLED


	// 입장한 플레이어가 방장일 경우
	if (PlayerID == HostID)
	{
		// 플레이어 추가 및 업데이트
		PlayerMap.Add(PlayerID, FPlayerLobbyData{ PlayerID, NickName, false, true });
		PlayerIDToSlotIndexMap.Add(PlayerID, 0);
		UpdateSlot(PlayerID);
		return;
	}
	// 입장한 플레이어가 방장이 아닌 경우
	else
	{

		// 최대 인원에 도달했는지 확인
		if (NextSlotIndex >= MaxPlayerCount)
			return;

		// 플레이어 추가 및 업데이트
		PlayerMap.Add(PlayerID, FPlayerLobbyData{ PlayerID, NickName, bReady, false });
		PlayerIDToSlotIndexMap.Add(PlayerID, NextSlotIndex++);
		UpdateSlot(PlayerID);
	}
	// 슬롯 업데이트
}

void USTLobbyStatusWidget::LeavePlayer(uint64 PlayerID)
{
	if (!PlayerMap.Contains(PlayerID))
		return;

	PlayerMap.Remove(PlayerID);
	PlayerIDToSlotIndexMap.Remove(PlayerID);

	RebuildSlots();
}

void USTLobbyStatusWidget::RebuildSlots()
{
	UE_LOG(LogTemp, Log, TEXT("Rebuilding lobby slots..."));

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

	// 슬롯 인덱스 재할당
	PlayerIDToSlotIndexMap.Empty();
	
	for (auto& [PlayerID, Data] : PlayerMap)
	{
		if (Data.bIsHost)
		{
			PlayerIDToSlotIndexMap.Add(PlayerID, 0);
		}
		else
		{
			if (NextSlotIndex >= MaxPlayerCount)
			{
				UE_LOG(LogTemp, Warning, TEXT("Max player count reached while rebuilding slots. Player ID %llu will not be displayed."), PlayerID);
				continue;
			}
			PlayerIDToSlotIndexMap.Add(PlayerID, NextSlotIndex++);
		}
	}
	UpdateSlot();
}

void USTLobbyStatusWidget::SetPlayerReady(uint64 PlayerID, bool bReady)
{
	FPlayerLobbyData* Data = PlayerMap.Find(PlayerID);
	if (!Data) {
		UE_LOG(LogTemp, Warning, TEXT("Player ID %llu not found in PlayerMap."), PlayerID);
		return;
	}

	Data->bReady = bReady;

	UpdateSlot(PlayerID);
}

void USTLobbyStatusWidget::UpdateSlot(const uint64 PlayerID)
{
	// 데이터 존재 여부 확인
	auto* DataPtr{ PlayerMap.Find(PlayerID) };
	if (nullptr == DataPtr) {
		UE_LOG(LogTemp, Warning, TEXT("Player ID %llu not found in PlayerMap."), PlayerID);
		return;
	}
	auto& Data{ *DataPtr };

	// 슬롯 인덱스 확인
	auto SlotIndexPtr{ PlayerIDToSlotIndexMap.Find(PlayerID) };
	if (nullptr == SlotIndexPtr) {
		UE_LOG(LogTemp, Warning, TEXT("Player ID %llu not found in PlayerIDToSlotIndexMap."), PlayerID);
		return;
	}
	auto SlotIndex{ *SlotIndexPtr };

	// 슬롯 업데이트
	switch (SlotIndex)
	{
	case 0:
		RoomOwnerNickName->SetText(FText::FromString(Data.NickName));
		RoomOwnerNickName->SetVisibility(ESlateVisibility::Visible);
		break;
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
	default:
		UE_LOG(LogTemp, Warning, TEXT("Invalid slot index %d for Player ID %llu."), SlotIndex, PlayerID);
		break;
	}
}

void USTLobbyStatusWidget::UpdateSlot()
{
	for (auto& [PlayerID, _] : PlayerMap)
	{
		UpdateSlot(PlayerID);
	}
}