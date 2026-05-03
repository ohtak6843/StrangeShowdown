// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SheriffTimer/STDangerAreaTimerWidget.h"
#include "Components/Image.h"

void USTDangerAreaTimerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::Hidden);

	if (TimerImage)
	{
		TimerMID = TimerImage->GetDynamicMaterial();
	}
}

void USTDangerAreaTimerWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (TimerMID)
	{
		CurrentTime += InDeltaTime;

		float Percent = MaxTime > 0.f ? CurrentTime / MaxTime : 0.f;

		Percent = FMath::Clamp(Percent, 0.f, 1.f);

		TimerMID->SetScalarParameterValue(TEXT("Progress"), Percent);
	}
}

void USTDangerAreaTimerWidget::SetTimer(float InMaxTime)
{
	SetVisibility(ESlateVisibility::Visible);
	bIsTimerActive = true;
	CurrentTime = 0.f;
	MaxTime = InMaxTime;
	HeartbeatCurrentTime = 0.f;
	HeartbeatMaxTime = 1.f;
}

void USTDangerAreaTimerWidget::ClearTimer()
{
	SetVisibility(ESlateVisibility::Hidden);
	bIsTimerActive = false;
	CurrentTime = 0.f;
	MaxTime = 0.f;
	HeartbeatCurrentTime = 0.f;
	HeartbeatMaxTime = 0.f;
}