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
	FMissionUpdate,
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
	void AddMission();

	UFUNCTION(BlueprintCallable)
	void ClearMission(int32 Index);

	UFUNCTION(BlueprintCallable)
	void UpdateMissionProgress(int32 Index, int32 ProgressToAdd);

	UFUNCTION(BlueprintCallable)
	void AddProgress(USTMissionRowData* Mission);

	UFUNCTION(BlueprintCallable)
	void TestMissionProgressUp();

public:
	UPROPERTY(BlueprintAssignable)
	FMissionStart OnMissionStart;

	UPROPERTY(BlueprintAssignable)
	FMissionClear OnMissionClear;

	UPROPERTY(BlueprintAssignable)
	FMissionUpdate OnMissionUpdate;

private:
	UPROPERTY()
	TArray<USTMissionRowData*> Missions;

public:
	USTMissionRowData* SetAttack();
	USTMissionRowData* SetGainGold();
	USTMissionRowData* SetDestroyDoor();
	USTMissionRowData* SetDamageSheriff();
	USTMissionRowData* SetSurviveWithoutAttack();
	USTMissionRowData* SetBuyItem();
	USTMissionRowData* SetTest();

	// 실제 미션 진행 함수
public:
	void OnAttack();
	void OnGainGold();
	void OnDestroyDoor();
	void OnDamageSheriff();
	void OnSurviveWithoutAttack();
	void OnBuyItem();
	void OnTest();
};