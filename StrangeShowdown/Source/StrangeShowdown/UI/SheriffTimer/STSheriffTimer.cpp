// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SheriffTimer/STSheriffTimer.h"
#include "Components/Image.h"

void USTSheriffTimer::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::Hidden);

	if (TimerImage)
	{
		TimerMID = TimerImage->GetDynamicMaterial();
	}
}

void USTSheriffTimer::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
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

void USTSheriffTimer::SetTimer(float InMaxTime)
{
	SetVisibility(ESlateVisibility::Visible);
	bIsTimerActive = true;
	CurrentTime = 0.f;
	MaxTime = InMaxTime;
	HeartbeatCurrentTime = 0.f;
	HeartbeatMaxTime = 1.f;
}

void USTSheriffTimer::ClearTimer()
{
	SetVisibility(ESlateVisibility::Hidden);
	bIsTimerActive = false;
	CurrentTime = 0.f;
	MaxTime = 0.f;
	HeartbeatCurrentTime = 0.f;
	HeartbeatMaxTime = 0.f;
}