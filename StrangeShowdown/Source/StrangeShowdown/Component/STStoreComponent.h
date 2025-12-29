#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "STStoreComponent.generated.h"

class ASTLocalPlayer;
class USTItemDataAssetBase;

USTRUCT(BlueprintType)
struct FStoreSlot
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	USTItemDataAssetBase* ItemData = nullptr;

	UPROPERTY(BlueprintReadWrite)
	int32 SlotIndex = -1;

	UPROPERTY(BlueprintReadWrite)
	bool bSold = false;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class STRANGESHOWDOWN_API USTStoreComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USTStoreComponent();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Store")
	TArray<USTItemDataAssetBase*> StoreItemPool;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Store")
	TArray<FStoreSlot> Slots;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Store")
	int32 SlotCount = 5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Store")
	int32 RerollCost = 5;

public:
	UFUNCTION(BlueprintCallable, Category = "Store")
	void InitStore();

	UFUNCTION(BlueprintCallable, Category = "Store")
	bool BuyItem(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "Store")
	void Reroll();

private:
	ASTLocalPlayer* OwnerPlayer = nullptr;
};