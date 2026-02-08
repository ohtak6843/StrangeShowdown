// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/STGameInstance.h"
#include "GameFramework/GameUserSettings.h"

void USTGameInstance::Init()
{
	Super::Init();

	// GameUserSettings 설정 로드
	UGameUserSettings* UserSettings = GEngine->GetGameUserSettings();
	if (UserSettings)
	{
		UserSettings->LoadSettings(false);
	}

	// 해상도
	if (UserSettings)
	{
		FIntPoint Resolution = UserSettings->GetScreenResolution();
		UE_LOG(LogTemp, Log, TEXT("Loaded Screen Resolution: %dx%d"), Resolution.X, Resolution.Y);
		ScreenResolution = Resolution;
	}

	// 창모드
	if (UserSettings)
	{
		EWindowMode::Type FullscreenMode = UserSettings->GetFullscreenMode();
		UE_LOG(LogTemp, Log, TEXT("Loaded Fullscreen Mode: %d"), static_cast<int32>(FullscreenMode));
		WindowMode = FullscreenMode;
	}
}

void USTGameInstance::AddRoom(USTRoomInfoObject* NewRoom)
{
	// 방 목록 추가
	RoomList.Add(NewRoom);

	// 방 목록이 업데이트되었음을 UI에게 알림
	OnRoomListUpdated.Broadcast();
}