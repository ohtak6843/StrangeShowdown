// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/STPlayerController.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "UI/STHUDWidget.h"
#include "Actor/STInteractableActor.h"
#include "DrawDebugHelpers.h"

ASTPlayerController::ASTPlayerController()
{
}

void ASTPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	EnhancedInputComponent->BindAction(OpenInventoryAction, ETriggerEvent::Triggered, this, &ASTPlayerController::OpenInventory);
	EnhancedInputComponent->BindAction(OpenStoreAction, ETriggerEvent::Triggered, this, &ASTPlayerController::OpenStore);
	EnhancedInputComponent->BindAction(FocusChatManagerAction, ETriggerEvent::Triggered, this, &ASTPlayerController::FocusChatManager);
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

void ASTPlayerController::FocusChatManager(const FInputActionValue& Value)
{
	if (HUDWidget)
	{
		HUDWidget->FocusChatManager();
	}
}
