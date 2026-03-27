#include "Widget/STMissionWidget.h"
#include "GameFramework/PlayerController.h"
#include "Character/Player/STLocalPlayer.h"
#include "Component/STMissionComponent.h"
#include "Game/STMissionRowData.h"

void USTMissionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (APlayerController* PC = GetOwningPlayer())
	{
		if (ASTLocalPlayer* LocalPlayer = Cast<ASTLocalPlayer>(PC->GetPawn()))
		{
			if (LocalPlayer->GetMissionComp())
			{
				LocalPlayer->GetMissionComp()->OnMissionUpdated.AddDynamic(
					this,
					&USTMissionWidget::AddMission
				);
			}
		}
	}
}

void USTMissionWidget::AddMission(const FText& NewTitle, const FText& NewMission)
{
	if (!MissionListView)
	{
		UE_LOG(LogTemp, Warning, TEXT("MissionListView is null"));
		return;
	}

	// 최대 개수 제한
	if (MissionItems.Num() >= MaxMissionCount)
	{
		UE_LOG(LogTemp, Warning, TEXT("Mission limit exceeded (max 3)"));
		return;
	}

	USTMissionRowData* NewItem = NewObject<USTMissionRowData>(this);
	if (!NewItem) return;

	NewItem->Init(NewTitle, NewMission);

	MissionItems.Add(NewItem);
	MissionListView->AddItem(NewItem);
}