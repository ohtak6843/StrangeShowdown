
#pragma once

#include "CoreMinimal.h"
#include "STSlotData.generated.h"

USTRUCT(BlueprintType)
struct FSTItemSlot
{
	GENERATED_BODY()

public:
	FSTItemSlot() : ItemData(nullptr), bIsCountable(true), Count(0) {}
	FSTItemSlot(class USTItemDataAssetBase* InItemData, bool bInIsCountable = true, int32 InCount = 1)
		: ItemData(InItemData), bIsCountable(bInIsCountable), Count(InCount) {}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class USTItemDataAssetBase> ItemData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 bIsCountable : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Count;
};