// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/STSheriffController.h"
#include "STSheriffController.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/InputComponent.h"
#include "UI/STHUDWidget.h"

ASTSheriffController::ASTSheriffController()
{
}

void ASTSheriffController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

}

void ASTSheriffController::BeginPlay()
{
	Super::BeginPlay();

	HUDWidget = CreateWidget<USTHUDWidget>(this, HUDWidgetClass);

	if (HUDWidget)
	{
		HUDWidget->AddToViewport();
	}
}

USTMiniMapWidget* ASTSheriffController::GetMiniMapWidget()
{
	USTMiniMapWidget* MiniMapWidget = ISTControllerHUDInterface::GetMiniMapWidget();

	if (HUDWidget)
	{
		MiniMapWidget = HUDWidget->GetMiniMapWidget();
	}

	return MiniMapWidget;
}

USTMiniMapWidget* ASTSheriffController::GetBigMapWidget()
{
	USTMiniMapWidget* BigMapWidget = ISTControllerHUDInterface::GetMiniMapWidget();

	if (HUDWidget)
	{
		BigMapWidget = HUDWidget->GetMiniMapWidget();
	}

	return BigMapWidget;
}

void ASTSheriffController::FocusChatManager(const FInputActionValue& Value)
{
	if (HUDWidget)
	{
		HUDWidget->FocusChatManager();
	}
}

void ASTSheriffController::OpenBigMap(const FInputActionValue& Value)
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

void ASTSheriffController::OpenBountyPoster(const FInputActionValue& Value)
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
