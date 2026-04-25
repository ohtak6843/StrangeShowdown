// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Component/STInventoryComponent.h"
#include "GameData/STSlotData.h"
#include "STQuickSlotComponent.generated.h"


DECLARE_MULTICAST_DELEGATE(FOnQuickSlotUpdated);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STRANGESHOWDOWN_API USTQuickSlotComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USTQuickSlotComponent();

	virtual void InitializeComponent() override;

public:
	void SetCurrentSelectIndex(int32 NewIndex);

	// 아이템 추가
	UFUNCTION(BlueprintCallable, Category = QuickSlot)
	bool AddToQuickSlot(USTInventoryComponent* InventorySystem, int32 InventoryItemIndex, int32 TargetQuickSlotIndex);

	UFUNCTION(BlueprintCallable, Category = QuickSlot)
	class USTItemDataAssetBase* GetCurrentSelectedQuickSlotItemData() const;

	bool AddItem(class USTInventoryComponent* InventoryComp, int32 InventoryIndex, int32 QuickSlotIndex);

public:
	FOnQuickSlotUpdated OnQuickSlotUpdated;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = QuickSlot)
	TArray<FSTItemSlot> QuickSlots;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = QuickSlot)
	TArray<int32> InventorySlotIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = QuickSlot)
	int32 QuickSlotSize = 6;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = QuickSlot)
	int32 CurrentSelectQuickSlotIndex;

private:
	// 고정 인덱스(빈 슬롯으로 사용 불가)
	int fixIndex = 2;
};
