#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "STMissionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FMissionUpdated,
	const FText&, Title,
	const FText&, Description
);

// 플레이어에 UI로 미션을 전달할 수 있도록 하는 컴포넌트
// 컴포넌트 내에서 미션의 정보를 저장하고 미션 클리어 여부를 관리할 수 있도록 할 예정
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class STRANGESHOWDOWN_API USTMissionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USTMissionComponent();

	UFUNCTION(BlueprintCallable, Category = "Mission")
	void SetMission(const FText& NewTitle, const FText& NewDesc);

public:
	UPROPERTY(BlueprintAssignable)
	FMissionUpdated OnMissionUpdated;

private:
	UPROPERTY()
	FText CurrentTitle;

	UPROPERTY()
	FText CurrentMission;
};