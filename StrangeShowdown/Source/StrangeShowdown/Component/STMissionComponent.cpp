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