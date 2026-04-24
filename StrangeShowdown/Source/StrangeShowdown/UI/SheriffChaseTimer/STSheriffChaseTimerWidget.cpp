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
}

void USTSheriffChaseTimerWidget::SetTimerWidgetLocation(FVector2D InLocation)
{
	SetRenderTranslation(InLocation);
}

void USTSheriffChaseTimerWidget::SetTimerWidgetProgress(float InCurrentTime, float InMaxTime)
{
	CurrentTime = InCurrentTime;
	MaxTime = InMaxTime;

	if (TimerMID)
	{
		float Percent = MaxTime > 0.f ? CurrentTime / MaxTime : 0.f;
		Percent = FMath::Clamp(Percent, 0.f, 1.f);
		TimerMID->SetScalarParameterValue(TEXT("Progress"), Percent);
	}
}