// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Component/STInventoryComponent.h"
#include "GameData/STItemSlot.h"
#include "STQuickSlotComponent.generated.h"


DECLARE_MULTICAST_DELEGATE(FOnQuickSlotUpdated);

// MouseDrop 이벤트 디스패처
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FMouseDropToQuickSlotEvent,
	int32, InventoryItemIndex,
	USTInventoryComponent*, BeforeInventorySystem,
	int32, QuickSlotIndex
);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STRANGESHOWDOWN_API USTQuickSlotComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USTQuickSlotComponent();

	virtual void InitializeComponent() override;

public:
	// 아이템 추가
	UFUNCTION(BlueprintCallable, Category = "QuickSlot")
	bool AddToQuickSlot(USTInventoryComponent* InventorySystem, int32 InventoryItemIndex, int32 TargetQuickSlotIndex);

	// BP에서 함수처럼 호출할 수 있는 MouseDrop wrapper
	UFUNCTION(BlueprintCallable, Category = "QuickSlot")
	void CallMouseDropToQuickSlot(int32 InventoryItemIndex, USTInventoryComponent* BeforeInventorySystem, int32 QuickSlotIndex);

	// MouseDropToQuickSlot에 의해 호출될 ChangeSlot 처리용 래퍼
	UFUNCTION()
	void AddToQuickSlot_FromEvent(int32 InventoryItemIndex, USTInventoryComponent* BeforeInventorySystem, int32 QuickSlotIndex);

	UFUNCTION(BlueprintCallable, Category = "QuickSlot")
	class USTItemDataAssetBase* GetCurrentSelectedQuickSlotItemData() const;

	void AddItem(const FSTItemSlot& ItemSlot, int32 InventoryIndex, int32 QuickSlotIndex);

public:
	// MouseDropToQuickSlot 이벤트 디스패처
	UPROPERTY(BlueprintAssignable, Category = "QuickSlot")
	FMouseDropToQuickSlotEvent MouseDropToQuickSlot;

	FOnQuickSlotUpdated OnQuickSlotUpdated;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuickSlot")
	TArray<FSTItemSlot> QuickSlots;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuickSlot")
	TArray<int32> InventorySlotIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuickSlot")
	int32 QuickSlotSize = 6;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuickSlot")
	int32 CurrentSelectQuickSlotIndex;

private:
	// 고정 인덱스(빈 슬롯으로 사용 불가)
	int fixIndex = 2;
};
