#include "Widget/STMissionWidget.h"
#include "GameFramework/PlayerController.h"
#include "Game/STPlayerState.h"
#include "Component/STMissionComponent.h"
#include "Game/STMissionRowData.h"

void USTMissionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (APlayerController* PC = GetOwningPlayer())
	{
		if (ASTPlayerState* PS = PC->GetPlayerState<ASTPlayerState>())
		{
			if (PS->MissionComponent)
			{
				// MissionComponent 델리게이트 바인딩
				PS->MissionComponent->OnMissionUpdated.AddDynamic(
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