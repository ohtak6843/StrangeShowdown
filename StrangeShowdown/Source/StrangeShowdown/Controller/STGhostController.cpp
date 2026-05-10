// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/STGhostController.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/InputComponent.h"
#include "UI/STHUDWidget.h"

ASTGhostController::ASTGhostController()
{
}

void ASTGhostController::Destroyed()
{
	if (HUDWidget)
	{
		HUDWidget->RemoveFromParent();
		HUDWidget = nullptr;
	}

	Super::Destroyed();
}

void ASTGhostController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

}

void ASTGhostController::BeginPlay()
{
	Super::BeginPlay();

	HUDWidget = CreateWidget<USTHUDWidget>(this, HUDWidgetClass);

	if (HUDWidget)
	{
		HUDWidget->AddToViewport();
	}
}

USTMiniMapWidget* ASTGhostController::GetMiniMapWidget()
{
	USTMiniMapWidget* MiniMapWidget = ISTControllerHUDInterface::GetMiniMapWidget();

	if (HUDWidget)
	{
		MiniMapWidget = HUDWidget->GetMiniMapWidget();
	}

	return MiniMapWidget;
}

USTMiniMapWidget* ASTGhostController::GetBigMapWidget()
{
	USTMiniMapWidget* BigMapWidget = ISTControllerHUDInterface::GetBigMapWidget();

	if (HUDWidget)
	{
		BigMapWidget = HUDWidget->GetBigMapWidget();
	}

	return BigMapWidget;
}

void ASTGhostController::FocusChatManager(const FInputActionValue& Value)
{
	if (HUDWidget)
	{
		HUDWidget->FocusChatManager();
	}
}

void ASTGhostController::OpenBigMap(const FInputActionValue& Value)
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

void ASTGhostController::OpenBountyPoster(const FInputActionValue& Value)
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
