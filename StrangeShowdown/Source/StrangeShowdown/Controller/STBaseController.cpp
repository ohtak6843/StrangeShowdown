// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/STBaseController.h"

ASTBaseController::ASTBaseController()
{
}

void ASTBaseController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);
}
