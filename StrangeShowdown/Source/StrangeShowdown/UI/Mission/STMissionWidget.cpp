#include "UI/Mission/STMissionWidget.h"
#include "GameFramework/PlayerController.h"
#include "Character/Player/STLocalPlayer.h"
#include "Component/STMissionComponent.h"

void USTMissionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 슬롯 구성
	MissionSlots =
	{
		{ Mission1Title, Mission1, Mission1Image },
		{ Mission2Title, Mission2, Mission2Image },
		{ Mission3Title, Mission3, Mission3Image }
	};

	// 초기 숨김
	for (auto& MissionSlot : MissionSlots)
	{
		if (MissionSlot.Title)   MissionSlot.Title->SetVisibility(ESlateVisibility::Hidden);
		if (MissionSlot.Mission) MissionSlot.Mission->SetVisibility(ESlateVisibility::Hidden);
		if (MissionSlot.Image)   MissionSlot.Image->SetVisibility(ESlateVisibility::Hidden);
	}

	// Delegate 바인딩
	if (APlayerController* PC = GetOwningPlayer())
	{
		if (ASTLocalPlayer* LocalPlayer = Cast<ASTLocalPlayer>(PC->GetPawn()))
		{
			if (USTMissionComponent* MissionComp = LocalPlayer->GetMissionComp())
			{
				MissionComp->OnMissionStart.AddDynamic(this, &USTMissionWidget::AddMission);
				MissionComp->OnMissionClear.AddDynamic(this, &USTMissionWidget::MissionClear);
			}
		}
	}
}

void USTMissionWidget::AddMission(USTMissionRowData* Data)
{
	if (!Data) return;

	if (ActiveMissions.Num() >= MissionSlots.Num())
		return;

	ActiveMissions.Add(Data);

	RebuildSlots();

	MissionAnimation(ActiveMissions.Num());
}

void USTMissionWidget::MissionClear(USTMissionRowData* Data)
{
	if (!Data) return;

	int32 Index = ActiveMissions.IndexOfByKey(Data);
	if (!ActiveMissions.IsValidIndex(Index))
		return;

	Data->bIsCleared = true;

	// 텍스트 변경
	if (MissionSlots[Index].Mission)
	{
		MissionSlots[Index].Mission->SetText(FText::FromString(TEXT("Mission Clear!")));
	}

	MissionAnimation(Index + 1);

	// 3초 후 제거
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		FTimerDelegate::CreateUObject(this, &USTMissionWidget::RemoveMission, Data),
		3.0f,
		false
	);
}

void USTMissionWidget::RemoveMission(USTMissionRowData* Data)
{
	if (!Data) return;

	ActiveMissions.Remove(Data);

	RebuildSlots();
}

void USTMissionWidget::RebuildSlots()
{
	// 전부 숨김
	for (auto& MissionSlot : MissionSlots)
	{
		if (MissionSlot.Title)   MissionSlot.Title->SetVisibility(ESlateVisibility::Hidden);
		if (MissionSlot.Mission) MissionSlot.Mission->SetVisibility(ESlateVisibility::Hidden);
		if (MissionSlot.Image)   MissionSlot.Image->SetVisibility(ESlateVisibility::Hidden);
	}

	// 데이터 기준 재배치
	for (int32 i = 0; i < ActiveMissions.Num(); ++i)
	{
		USTMissionRowData* Data = ActiveMissions[i];
		if (!Data) continue;

		FMissionSlot& MissionSlot = MissionSlots[i];

		if (MissionSlot.Title)
		{
			MissionSlot.Title->SetText(Data->Title);
			MissionSlot.Title->SetVisibility(ESlateVisibility::Visible);
		}

		if (MissionSlot.Mission)
		{
			if (Data->bIsCleared)
			{
				MissionSlot.Mission->SetText(FText::FromString(TEXT("Mission Clear!")));
			}
			else
			{
				MissionSlot.Mission->SetText(Data->Mission);
			}
			MissionSlot.Mission->SetVisibility(ESlateVisibility::Visible);
		}

		if (MissionSlot.Image)
		{
			MissionSlot.Image->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void USTMissionWidget::MissionAnimation_Implementation(int32 Index)
{
	// 블루프린트에서 구현
}