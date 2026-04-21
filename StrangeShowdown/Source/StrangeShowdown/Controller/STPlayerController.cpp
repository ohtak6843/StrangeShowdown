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
	EnhancedInputComponent->BindAction(OpenBigMapAction, ETriggerEvent::Triggered, this, &ASTPlayerController::OpenBigMap);
	EnhancedInputComponent->BindAction(OpenBountyPosterAction, ETriggerEvent::Triggered, this, &ASTPlayerController::OpenBountyPoster);
}

void ASTPlayerController::BeginPlay()
{
	Super::BeginPlay();

	HUDWidget = CreateWidget<USTHUDWidget>(this, HUDWidgetClass);

	if (HUDWidget)
	{
		HUDWidget->AddToViewport();
	}
}

USTMiniMapWidget* ASTPlayerController::GetMiniMapWidget()
{
	USTMiniMapWidget* MiniMapWidget = ISTControllerHUDInterface::GetMiniMapWidget();

	if (HUDWidget)
	{
		MiniMapWidget = HUDWidget->GetMiniMapWidget();
	}
	
	return MiniMapWidget;
}

USTMiniMapWidget* ASTPlayerController::GetBigMapWidget()
{
	USTMiniMapWidget* BigMapWidget = ISTControllerHUDInterface::GetBigMapWidget();
	
	if (HUDWidget)
	{
		BigMapWidget = HUDWidget->GetBigMapWidget();
	}

	return BigMapWidget;
}

void ASTPlayerController::OpenStore()
{
	if (HUDWidget)
	{
		if (!bIsStoreOpen)
		{
			bool Result = HUDWidget->OpenStoreMenu();
			bIsStoreOpen = Result;
		}
	}
}

void ASTPlayerController::OpenInventory(const FInputActionValue& Value)
{
	if (HUDWidget)
	{
		if(bIsInventoryOpen)
		{
			bool Result = HUDWidget->CloseInventoryMenu();
			bIsInventoryOpen = !Result;
		}
		else
		{
			bool Result = HUDWidget->OpenInventoryMenu();
			bIsInventoryOpen = Result;
		}
	}
}

void ASTPlayerController::FocusChatManager(const FInputActionValue& Value)
{
	if (HUDWidget)
	{
		HUDWidget->FocusChatManager();
	}
}

void ASTPlayerController::OpenBigMap(const FInputActionValue& Value)
{
	if (HUDWidget)
	{
		if (bIsBigMapOpen)
		{
			bool result = HUDWidget->CloseBigMap();
			bIsBigMapOpen = !result;
		}
		else
		{
			bool result = HUDWidget->OpenBigMap();
			bIsBigMapOpen = result;
		}
	}
}

void ASTPlayerController::OpenBountyPoster(const FInputActionValue& Value)
{
	if (HUDWidget)
	{
		if (bIsBountyPosterOpen)
		{
			bool result = HUDWidget->CloseBountyPoster();
			bIsBountyPosterOpen = !result;
		}
		else
		{
			bool result = HUDWidget->OpenBountyPoster();
			bIsBountyPosterOpen = result;
		}
	}
}
