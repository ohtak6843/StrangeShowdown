#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/WidgetComponent.h"
#include "GameData/STItemSlot.h"
#include "STInventoryComponent.generated.h"

UENUM(BlueprintType)
enum class EItemUseType : uint8
{
	CanUse,
	NotEnoughStaminaCost,
	NoEffect,
	Exception,
	UnValid
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnInventoryUpdated, const TArray<FSTItemSlot>& /*InItemSlots*/);

// MouseDrop 이벤트 디스패처
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FMouseDropEvent,
	int32, TargetIndex,
	USTInventoryComponent*, BeforeInventorySystem,
	int32, BeforeIndex
);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class STRANGESHOWDOWN_API USTInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USTInventoryComponent();

	virtual void InitializeComponent() override;

public:
	// 아이템 추가
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddItem(USTItemDataAssetBase* NewItem, int32 Count, int32& OutAddedInventoryIndex);

	// 아이템 제거
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItem(int32 SlotIndex, int32 Count = 1);

	// 아이템 사용
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	EItemUseType UseItem(int32 SlotIndex, ASTLocalPlayer* Player, int32 StaminaCost, FSTItemSlot& OutSlot);

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

	FOnInventoryUpdated OnInventoryUpdated;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TArray<FSTItemSlot> Slots;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 MaxSlots = 8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 CurrentSlotCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 InventorySize = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	bool bIsAddFailed = false;
};