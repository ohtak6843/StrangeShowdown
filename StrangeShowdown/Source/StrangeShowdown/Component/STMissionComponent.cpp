#include "Component/STMissionComponent.h"
#include "Net/UnrealNetwork.h"

USTMissionComponent::USTMissionComponent()
{
	SetIsReplicatedByDefault(true);
}

void USTMissionComponent::SetMission(const FText& Title, const FText& Mission)
{
	CurrentTitle = Title;
	CurrentMission = Mission;

	OnMissionUpdated.Broadcast(CurrentTitle, CurrentMission);
}