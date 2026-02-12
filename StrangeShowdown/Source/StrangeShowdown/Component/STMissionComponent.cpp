#include "Component/STMissionComponent.h"
#include "Net/UnrealNetwork.h"

USTMissionComponent::USTMissionComponent()
{
	SetIsReplicatedByDefault(true);
}

void USTMissionComponent::SetMission(const FText& NewTitle, const FText& NewMission)
{
	CurrentTitle = NewTitle;
	CurrentMission = NewMission;

	OnMissionUpdated.Broadcast(CurrentTitle, CurrentMission);
}