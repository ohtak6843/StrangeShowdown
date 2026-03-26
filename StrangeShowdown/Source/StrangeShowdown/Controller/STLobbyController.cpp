// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/STLobbyController.h"

void ASTLobbyController::BeginPlay()
{
	Super::BeginPlay();
}

void ASTLobbyController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASTLobbyController::SetReady(bool bReady)
{
	bIsReady = bReady;
	
	// TODO: 서버에 레디 상태 전달
}