#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Character/NPC/STStorekeeper.h"
#include "STStoreComponent.generated.h"

class ASTLocalPlayer;
class USTItemDataAssetBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStoreUpdated);

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
	ASTStorekeeper* CurrentStorekeeper;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Store")
	TArray<FStoreSlot> Slots;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Store")
	int32 SlotCount = 5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Store")
	int32 RerollCost = 5;

	// UI 갱신 이벤트
	UPROPERTY(BlueprintAssignable)
	FOnStoreUpdated OnStoreUpdated;

public:
	UFUNCTION(BlueprintCallable, Category = "Store")
	void InitStore();

	UFUNCTION(BlueprintCallable, Category = "Store")
	void Reroll();

	UFUNCTION(BlueprintCallable, Category = "Store")
	void BuyItem(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "Store")
	void RefreshStoreUI();

	const TArray<USTItemDataAssetBase*>& GetStoreItems() const
	{
		return CurrentStorekeeper->StoreItemPool;
	}

private:
	ASTLocalPlayer* OwnerPlayer = nullptr;
};