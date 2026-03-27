
#pragma once

#include "CoreMinimal.h"
#include "STItemSlot.generated.h"

USTRUCT(BlueprintType)
struct FSTItemSlot
{
	GENERATED_BODY()

public:
	FSTItemSlot() : ItemData(nullptr), bIsCountable(false), Count(0) {}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class USTItemDataAssetBase> ItemData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 bIsCountable : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Count;
};