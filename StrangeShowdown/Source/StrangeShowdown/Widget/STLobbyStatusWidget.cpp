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
	
	// if 방장
	// 닉네임도 설정
	AddRoomOwnerSlot(0, TEXT("RoomOwner"));

	// else if 방장이 아닌 경우 현재 방에 있는 모든 플레이어의 슬롯 추가
	// 닉네임도 설정
	AddPlayerSlot(1, TEXT("Player1"));

	// 여기서 직접 추가하는 것보다 컨트롤러에서 정보를 보내
	// 리스트를 추가하는 방법도 고려해볼 필요가 있음
	// 방장인지 아닌지는 컨트롤러에서 판단중
}

void USTLobbyStatusWidget::AddPlayerSlot(uint64 PlayerID, const FString& NickName)
{
	// WrapBox에 슬롯 추가
	if (SlotWidgetClass)
	{
		if (USTLobbyStatusSlotWidget* SlotWidget = CreateWidget<USTLobbyStatusSlotWidget>(GetWorld(), SlotWidgetClass))
		{
			SlotWidget->Init(PlayerID, NickName);
			WrapBox->AddChild(SlotWidget);
			SlotMap.Add(PlayerID, SlotWidget);
		}
	}
}

void USTLobbyStatusWidget::AddRoomOwnerSlot(uint64 PlayerID, const FString& NickName)
{
	// WrapBox에 슬롯 추가
	if (SlotWidgetClass)
	{
		if (USTLobbyStatusSlotWidget* SlotWidget = CreateWidget<USTLobbyStatusSlotWidget>(GetWorld(), SlotWidgetClass))
		{
			SlotWidget->Init(PlayerID, NickName);
			SlotWidget->ReadyIcon->SetBrushFromTexture(LoadObject<UTexture2D>(nullptr, TEXT("/Script/Engine.Texture2D'/Game/StrangeShowdown/UI/Texture/T_RoomOwnerIcon.T_RoomOwnerIcon'")));
			WrapBox->AddChild(SlotWidget);
			SlotMap.Add(PlayerID, SlotWidget);
		}
	}

	// 방장은 항상 Ready
	SetPlayerReady(PlayerID, true);
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