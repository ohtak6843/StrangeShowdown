// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Store/STStoreMenuWidget.h"
#include "UI/Store/STStoreWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

USTStoreMenuWidget::USTStoreMenuWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void USTStoreMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ExitButton)
	{
		ExitButton->OnHovered.AddDynamic(this, &USTStoreMenuWidget::HandleExitButtonHovered);
		ExitButton->OnUnhovered.AddDynamic(this, &USTStoreMenuWidget::HandleExitButtonUnhovered);
		ExitButton->OnClicked.AddDynamic(this, &USTStoreMenuWidget::HandleExitButtonClicked);
	}
}

void USTStoreMenuWidget::HandleExitButtonHovered()
{
	if (ExitButton)
	{
		ExitButton->SetColorAndOpacity(FLinearColor::Red);
	}

	if (ExitHoverAnimation)
	{
		PlayAnimation(ExitHoverAnimation, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f);
	}

	if (HoverSound)
	{
		float VolumeMultiplier = 0.8f;
		UGameplayStatics::PlaySound2D(this, HoverSound, VolumeMultiplier, 1.0f, 0.0f, nullptr, nullptr, true);
	}
}

void USTStoreMenuWidget::HandleExitButtonUnhovered()
{
	if (ExitButton)
	{
		ExitButton->SetColorAndOpacity(FLinearColor::White);
	}

	if (ExitHoverAnimation)
	{
		PlayAnimation(ExitHoverAnimation, 0.0f, 1, EUMGSequencePlayMode::Reverse, 1.0f);
	}
}

void USTStoreMenuWidget::HandleExitButtonClicked()
{
	if(OnExitButtonClicked.IsBound())
	{
		bool Result = OnExitButtonClicked.Execute();
	}
}
