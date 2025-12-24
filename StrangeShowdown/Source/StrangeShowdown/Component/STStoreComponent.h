// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item/STItemDataAssetBase.h"
#include "Character/STLocalPlayer.h"
#include "STStoreComponent.generated.h"

USTRUCT(BlueprintType)
struct FStoreItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USTItemDataAssetBase* ItemData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Price;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bSold = false;
};



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STRANGESHOWDOWN_API USTStoreComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USTStoreComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FStoreItem> CurrentItems;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 RerollCost = 5;

	UFUNCTION(BlueprintCallable)
	void InitializeStore();

	UFUNCTION(BlueprintCallable)
	bool BuyItem(int32 Index);

	UFUNCTION(BlueprintCallable)
	void Reroll();

	// Owner Player
	UPROPERTY()
	ASTLocalPlayer* OwnerPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Store")
	TArray<USTItemDataAssetBase*> StoreItemPool;
};
