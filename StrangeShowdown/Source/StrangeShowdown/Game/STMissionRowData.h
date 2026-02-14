#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "STMissionRowData.generated.h"

UCLASS(BlueprintType)
class STRANGESHOWDOWN_API USTMissionRowData : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly)
	FText Title;

	UPROPERTY(BlueprintReadOnly)
	FText Mission;

	void Init(const FText& InTitle, const FText& InMission)
	{
		Title = InTitle;
		Mission = InMission;
	}
};
