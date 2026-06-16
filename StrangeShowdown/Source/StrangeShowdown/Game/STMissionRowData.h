#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "STMissionRowData.generated.h"

UENUM(BlueprintType)
enum class EMissionType : uint8
{
	Attack,
	GainGold,
	DestroyDoor,
	DamageSheriff,
	SurviveWithoutAttack,
	BuyItem,
	Test,
};

UCLASS(BlueprintType)
class STRANGESHOWDOWN_API USTMissionRowData : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly)
	FText Title;

	UPROPERTY(BlueprintReadOnly)
	FText Mission;

	UPROPERTY(BlueprintReadOnly)
	FText MissionProgress;

	EMissionType MissionType;

	int32 CurrentProgress = 0;

	int32 MaxProgress = 0;

	UPROPERTY(BlueprintReadOnly)
	bool bIsCleared = false;

	void Init(const FText& InTitle, const FText& InMission, EMissionType InMissionType, int32 InCurrentProgress, int32 InMaxProgress)
	{
		Title = InTitle;
		Mission = InMission;
		MissionType = InMissionType;
		CurrentProgress = InCurrentProgress;
		MaxProgress = InMaxProgress;
		MissionProgress = FText::FromString(FString::Printf(TEXT("진행도: %d / %d"), CurrentProgress, MaxProgress));
	}
};
