#include "Component/STMissionComponent.h"
#include "Net/UnrealNetwork.h"

USTMissionComponent::USTMissionComponent()
{
	SetIsReplicatedByDefault(true);
}

void USTMissionComponent::SetMission(const FText& Title, const FText& Desc)
{
	USTMissionRowData* NewData = NewObject<USTMissionRowData>(this);
	if (!NewData) return;

	NewData->Init(Title, Desc);

	Missions.Add(NewData);

	OnMissionStart.Broadcast(NewData);
}

void USTMissionComponent::ClearMission(int32 Index)
{
	if (!Missions.IsValidIndex(Index))
		return;

	USTMissionRowData* Data = Missions[Index];

	OnMissionClear.Broadcast(Data);

	Missions.RemoveAt(Index);
}

void USTMissionComponent::SetTestMission(int32 Index)
{
	SetMission(FText::FromString(FString::Printf(TEXT("Mission %d"), Index)), FText::FromString(TEXT("This is a test mission.")));
}