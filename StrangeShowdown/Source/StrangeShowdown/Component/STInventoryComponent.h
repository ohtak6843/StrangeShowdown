#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "STInventoryComponent.generated.h"

USTRUCT(BlueprintType)
struct FInventorySlot
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USTItemDataAssetBase* ItemData = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Count = 0;
};

// MouseDrop 이벤트 디스패처
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FMouseDropEvent,
	int32, TargetIndex,
	USTInventoryComponent*, BeforeInventorySystem,
	int32, BeforeIndex
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class STRANGESHOWDOWN_API USTInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USTInventoryComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 아이템 추가
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddItem(USTItemDataAssetBase* NewItem, int32 Count = 1);

	// 아이템 제거
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItem(int32 SlotIndex, int32 Count = 1);

	// 아이템 사용
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool UseItem(int32 SlotIndex, class ASTLocalPlayer* Player);

	// 슬롯 교환
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool ChangeSlot(int32 SlotAIndex, int32 SlotBIndex, USTInventoryComponent* BeforeInventorySystem = nullptr);

	// 변경 UI 갱신 이벤트
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory")
	void OnSlotChanged(int32 SlotA, int32 SlotB);

	// BP에서 함수처럼 호출할 수 있는 MouseDrop wrapper
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void CallMouseDrop(int32 TargetIndex, USTInventoryComponent* BeforeInventorySystem, int32 BeforeIndex);

	// MouseDrop에 의해 호출될 ChangeSlot 처리용 래퍼
	UFUNCTION()
	void ChangeSlot_FromEvent(int32 TargetIndex, USTInventoryComponent* BeforeInventorySystem, int32 BeforeIndex);

private:
	int32 FindEmptySlot() const;
	int32 FindStackableSlot(USTItemDataAssetBase* NewItem) const;

public:
	// MouseDrop 이벤트 디스패처
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FMouseDropEvent MouseDrop;

	// OnInventoryUpdated 이벤트 디스패처
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryUpdated OnInventoryUpdated;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TArray<FInventorySlot> Slots;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 MaxSlots = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 CurrentSlotCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 InventorySize = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	bool bIsAddFailed = false;
};