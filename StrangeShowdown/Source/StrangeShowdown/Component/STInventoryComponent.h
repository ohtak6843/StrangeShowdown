#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/WidgetComponent.h"
#include "GameData/STSlotData.h"
#include "GameData/STTypes.h"
#include "STInventoryComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnInventoryUpdated);

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
	bool AddItem(FSTItemSlot ItemSlot, int32& OutAddedInventoryIndex);

	// 아이템 제거
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItem(int32 SlotIndex, int32 Count = 1);

	// 아이템 사용
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	EItemUseType UseItem(int32 SlotIndex, ASTLocalPlayer* Player, int32 StaminaCost, FSTItemSlot& OutSlot);

	// 슬롯 교환
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool ChangeSlot(int32 SlotAIndex, int32 SlotBIndex, USTInventoryComponent* BeforeInventorySystem = nullptr);

private:
	int32 FindEmptySlot() const;
	int32 FindStackableSlot(class USTItemDataAssetBase* NewItem) const;

public:
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