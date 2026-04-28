// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SheriffChaseTimer/STSheriffChaseTimerWidget.h"
#include "Components/Image.h"

void USTSheriffChaseTimerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::Hidden);

	if (TimerImage)
	{
		TimerMID = TimerImage->GetDynamicMaterial();
	}
}

void USTSheriffChaseTimerWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (TimerMID && bIsTimerActive)
	{
		CurrentTime += InDeltaTime;

		float Percent = MaxTime > 0.f ? 1.f - (CurrentTime / MaxTime) : 0.f;

		Percent = FMath::Clamp(Percent, 0.f, 1.f);

		TimerMID->SetScalarParameterValue(TEXT("Progress"), Percent);

		if (CurrentTime >= MaxTime)
		{
			bIsTimerActive = false;
		}
	}
}

void USTSheriffChaseTimerWidget::SetTimer(float InMaxTime)
{
	CurrentTime = 0.f;
	MaxTime = InMaxTime;
	bIsTimerActive = true;
}

void USTSheriffChaseTimerWidget::SetTimerWidgetLocation(FVector2D InLocation)
{
	SetRenderTranslation(InLocation);
}