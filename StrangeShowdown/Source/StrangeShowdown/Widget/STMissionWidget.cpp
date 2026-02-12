// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/STMissionWidget.h"
#include "GameFramework/PlayerController.h"
#include "Game/STPlayerState.h"
#include "Component/STMissionComponent.h"

void USTMissionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (APlayerController* PC = GetOwningPlayer())
	{
		if (ASTPlayerState* PS = PC->GetPlayerState<ASTPlayerState>())
		{
			if (PS->MissionComponent)
			{
				// 미션이 바뀌면 MissionWidget의 SetMission 함수 호출
				// 브로드캐스트 델리게이트에 함수 바인딩
				PS->MissionComponent->OnMissionUpdated.AddDynamic(
					this,
					&USTMissionWidget::SetMission
				);
			}
		}
	}
}

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

	// 블루프린트에서 구현된 함수 호출
	OnMissionUpdated();
}