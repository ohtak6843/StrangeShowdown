// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/STPlayerController.h"

void ASTPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);
}
