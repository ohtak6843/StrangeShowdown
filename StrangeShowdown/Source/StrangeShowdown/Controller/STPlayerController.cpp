// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/STPlayerController.h"
#include "UI/STHUDWidget.h"
#include "Actor/STInteractableActor.h"
#include "DrawDebugHelpers.h"

ASTPlayerController::ASTPlayerController()
{
}

void ASTPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (HUDWidgetClass)
	{
		HUDWidget = CreateWidget<USTHUDWidget>(this, HUDWidgetClass);
	}

	if (HUDWidget)
	{
		HUDWidget->AddToViewport();
	}
}

void ASTPlayerController::OpenInventory(const FInputActionValue& Value)
{
	if (HUDWidget)
	{
		//HUDWidget->OpenInventoryMenu();
	}
}

void ASTPlayerController::OpenStore(const FInputActionValue& Value)
{

}
