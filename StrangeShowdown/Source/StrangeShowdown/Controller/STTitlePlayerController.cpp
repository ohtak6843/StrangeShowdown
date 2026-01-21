// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/STTitlePlayerController.h"

void ASTTitlePlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameAndUI Mode;
	Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	Mode.SetHideCursorDuringCapture(false);
	SetInputMode(Mode);
	bShowMouseCursor = true;
}

void ASTTitlePlayerController::AddRoomToList(URoomInfoObject* NewRoom)
{
	RoomList.Add(NewRoom);
}
