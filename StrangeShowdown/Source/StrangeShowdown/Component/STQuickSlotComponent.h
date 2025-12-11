// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "STInventoryComponent.h"
#include "STQuickSlotComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnQuickSlotUpdated);

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

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 아이템 추가
	UFUNCTION(BlueprintCallable, Category = "QuickSlot")
	bool AddToQuickSlot(USTInventoryComponent* InventorySystem, int32 InventoryItemIndex, int32 TargetQuickSlotIndex);

	// MouseDropToQuickSlot 이벤트 디스패처
	UPROPERTY(BlueprintAssignable, Category = "QuickSlot")
	FMouseDropToQuickSlotEvent MouseDropToQuickSlot;

	// OnQuickSlotUpdated 이벤트 디스패처
	UPROPERTY(BlueprintAssignable, Category = "QuickSlot")
	FOnQuickSlotUpdated OnQuickSlotUpdated;

	// BP에서 함수처럼 호출할 수 있는 MouseDrop wrapper
	UFUNCTION(BlueprintCallable, Category = "QuickSlot")
	void CallMouseDropToQuickSlot(int32 InventoryItemIndex, USTInventoryComponent* BeforeInventorySystem, int32 QuickSlotIndex);

	// MouseDropToQuickSlot에 의해 호출될 ChangeSlot 처리용 래퍼
	UFUNCTION()
	void AddToQuickSlot_FromEvent(int32 InventoryItemIndex, USTInventoryComponent* BeforeInventorySystem, int32 QuickSlotIndex);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuickSlot")
	TArray<FInventorySlot> QuickSlots;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuickSlot")
	int32 QuickSlotSize = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuickSlot")
	int32 InventorySlotIndex;
};
