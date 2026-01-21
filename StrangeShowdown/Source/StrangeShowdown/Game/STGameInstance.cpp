// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/STGameInstance.h"

void USTGameInstance::AddRoomToList(URoomInfoObject* NewRoom)
{
	RoomList.Add(NewRoom);
}
