#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Character/NPC/STStorekeeper.h"
#include "STStoreComponent.generated.h"

class ASTLocalPlayer;
class USTItemDataAssetBase;

DECLARE_MULTICAST_DELEGATE(FOnStoreUpdated);

USTRUCT(BlueprintType)
struct FStoreSlot
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USTItemDataAssetBase* ItemData = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 SlotIndex = -1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsSold = false;
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
	// UI 갱신 이벤트
	FOnStoreUpdated OnStoreUpdated;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Store)
	TWeakObjectPtr<class ASTStorekeeper> CurrentStorekeeper;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Store)
	TArray<FStoreSlot> Slots;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Store)
	int32 SlotCount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Store)
	int32 RerollCost;

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