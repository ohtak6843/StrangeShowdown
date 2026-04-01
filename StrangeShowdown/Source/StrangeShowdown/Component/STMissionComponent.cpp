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

	// MissionWidget UI에 미션 정보 업데이트 알림(Add Mission 함수 호출)
	OnMissionUpdated.Broadcast(CurrentTitle, CurrentMission);
}