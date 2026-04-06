#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Game/STMissionRowData.h"
#include "STMissionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FMissionStart,
	USTMissionRowData*, Data
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FMissionClear,
	USTMissionRowData*, Data
);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class STRANGESHOWDOWN_API USTMissionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USTMissionComponent();

	UFUNCTION(BlueprintCallable)
	void SetMission(const FText& Title, const FText& Desc);

	UFUNCTION(BlueprintCallable)
	void ClearMission(int32 Index);

public:
	UPROPERTY(BlueprintAssignable)
	FMissionStart OnMissionStart;

	UPROPERTY(BlueprintAssignable)
	FMissionClear OnMissionClear;

private:
	UPROPERTY()
	TArray<USTMissionRowData*> Missions;
};