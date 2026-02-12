// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/STMissionWidget.h"

void USTMissionWidget::SetMission(const FText& NewTitle, const FText& NewMission)
{
	if (MissionTitle)
	{
		MissionTitle->SetText(NewTitle);
	}
	if (MissionText)
	{
		MissionText->SetText(NewMission);
	}
}