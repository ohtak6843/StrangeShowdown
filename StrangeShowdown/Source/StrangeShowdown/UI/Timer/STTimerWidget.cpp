// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Timer/STTimerWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void USTTimerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 타이머 초기화(임시로 Construct시 100초로 설정)
	SetTimer(100.f);

	if (TimerImage)
	{
		TimerMID = TimerImage->GetDynamicMaterial();
	}
}

void USTTimerWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
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

void USTTimerWidget::SetTimer(float InMaxTime)
{
	CurrentTime = 0.f;
	MaxTime = InMaxTime;
}

void USTTimerWidget::SetTurn(int InTurn)
{
	if (TurnText)
	{
		TurnText->SetText(FText::Format(NSLOCTEXT("STTimerWidget", "TurnFormat", "{0}턴"), FText::AsNumber(InTurn)));
	}
}