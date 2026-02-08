// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/STTitlePlayerController.h"
#include "Game/STGameInstance.h"

void ASTTitlePlayerController::BeginPlay()
{
	Super::BeginPlay();

	USTGameInstance* GI = GetGameInstance<USTGameInstance>();
	if (!IsValid(GI))
	{
		UE_LOG(LogTemp, Error, TEXT("GameInstance invalid"));
		return;
	}

	FInputModeGameAndUI Mode;
	Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	Mode.SetHideCursorDuringCapture(false);
	SetInputMode(Mode);
	bShowMouseCursor = true;
}

void ASTTitlePlayerController::SetRoomInfo(USTRoomInfoObject* NewRoom)
{
	RoomInfo = NewRoom;
	AddRoomToList(NewRoom);
}

void ASTTitlePlayerController::AddRoomToList(USTRoomInfoObject* NewRoom)
{
	USTGameInstance* GameInstance = Cast<USTGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		// 게임 인스턴스의 RoomList에 방 정보를 추가하는 함수를 호출
		GameInstance->AddRoom(NewRoom);
	}

	// 게임 인스턴스로 전달 후 RoomInfo 초기화
	RoomInfo = nullptr;
}

void ASTTitlePlayerController::CreateTestRoom()
{
	static int tempRoomID = 1;
	USTRoomInfoObject* TestRoom = NewObject<USTRoomInfoObject>();
	TestRoom->RoomInfo.RoomName = FString::Printf(TEXT("Test Room %d"), tempRoomID++);
	TestRoom->RoomInfo.CurrentPlayers = 3;
	TestRoom->RoomInfo.MaxPlayers = 5;
	TestRoom->RoomInfo.bHasPassword = true;
	TestRoom->RoomInfo.Password = TEXT("1234");
	AddRoomToList(TestRoom);

	// 로그
	UE_LOG(LogTemp, Log, TEXT("Test room created: %s"), *TestRoom->RoomInfo.RoomName);
}
