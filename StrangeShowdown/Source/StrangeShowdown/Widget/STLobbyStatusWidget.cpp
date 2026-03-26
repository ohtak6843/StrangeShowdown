// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/STLobbyStatusWidget.h"
#include "Game/STGameState.h"
#include "GameFramework/PlayerState.h"
#include "Widget/STLobbyStatusSlotWidget.h"

void USTLobbyStatusWidget::NativeConstruct()
{
	Super::NativeConstruct();

	WrapBox->ClearChildren();

	// 자신이 방장일 경우 초기화 시 본인 슬롯을 추가
	// 아래 함수에 방장 ID 넣으면 됨
	AddPlayerSlot(0);

	// 방장이 아닌 경우 현재 방에 있는 모든 플레이어의 슬롯 추가

}

void USTLobbyStatusWidget::AddPlayerSlot(uint64 PlayerID)
{
	// WrapBox에 슬롯 추가
	if (SlotWidgetClass)
	{
		if (USTLobbyStatusSlotWidget* SlotWidget = CreateWidget<USTLobbyStatusSlotWidget>(GetWorld(), SlotWidgetClass))
		{
			SlotWidget->Init(PlayerID);
			WrapBox->AddChild(SlotWidget);
			SlotMap.Add(PlayerID, SlotWidget);
		}
	}
}

void USTLobbyStatusWidget::SetPlayerReady(uint64 PlayerID, bool bReady)
{
	if (USTLobbyStatusSlotWidget** SlotWidgetPtr = SlotMap.Find(PlayerID))
	{
		if (USTLobbyStatusSlotWidget* SlotWidget = *SlotWidgetPtr)
		{
			SlotWidget->Ready(bReady);
		}
	}
}