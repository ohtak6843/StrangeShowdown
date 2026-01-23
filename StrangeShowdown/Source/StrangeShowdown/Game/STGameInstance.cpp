// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/STGameInstance.h"

void USTGameInstance::AddRoom(URoomInfoObject* NewRoom)
{
	// 방 목록 추가
	RoomList.Add(NewRoom);

	// 방 목록이 업데이트되었음을 UI에게 알림
	OnRoomListUpdated.Broadcast();
}