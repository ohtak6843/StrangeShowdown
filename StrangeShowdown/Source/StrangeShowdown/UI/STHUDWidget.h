// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameData/STItemSlot.h"
#include "Widget/STHUD.h"
#include "STHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API USTHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	USTHUDWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

public:
	class USTStatWidget* GetStatWidget() const { return StatWidget; }

	void UpdateMission();
	void UpdateMiniMap();
	void UpdateWorldMap();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD)
	EHUDWidgetType HUDWidgetType = EHUDWidgetType::Player;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD)
	TObjectPtr<class USTMissionWidget> Mission;


#pragma region Stat Section
public:
	void UpdateStat();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD, meta = (BindWidget))
	TObjectPtr<class USTStatWidget> StatWidget;
#pragma endregion

#pragma region Inventory Section
public:
	void ShowInventoryMenu(const TArray<FSTItemSlot>& InItemSlots);
	void UpdateInventoryMenu(const TArray<FSTItemSlot>& InItemSlots);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	TSubclassOf<class USTInventoryMenuWidget> InventoryMenuClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD)
	TObjectPtr<class USTInventoryMenuWidget> InventoryMenuWidget;
#pragma endregion

#pragma region QuickSlot Section
public:
	class USTQuickSlotWidget* GetQuickSlotWidget(int32 Index);

	void SetupQuickSlots(int32 QuickSlotCount);
	void UpdateQuickSlots(const TArray<FSTItemSlot>& InItemSlots, int32 CurrentSelectedIndex);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	TSubclassOf<class USTQuickSlotWidget> QuickSlotWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD, meta = (BindWidget))
	TObjectPtr<class UWrapBox> QuickSlotWrapBox;
#pragma endregion

#pragma region Store Section
public:

protected:
	// TODO: StoreMenuWidget 추가하기
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD)
	//TObjectPtr<class USTStoreMenuWidget> StoreMenu;

#pragma endregion

#pragma region Chat Section
public:

protected:

#pragma endregion

#pragma region Bounty Board Section
public:

protected:

#pragma endregion

#pragma region Map Section
public:

protected:

#pragma endregion
};
