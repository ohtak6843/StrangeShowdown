#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "STMissionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FMissionUpdated,
	const FText&, Title,
	const FText&, Description
);

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