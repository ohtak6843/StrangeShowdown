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
	void UpdateMission();
	void UpdateMiniMap();
	void UpdateWorldMap();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD)
	EHUDWidgetType HUDWidgetType = EHUDWidgetType::Player;

	// TODO: StoreMenuWidget 추가하기
	//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD)
	//TObjectPtr<class USTStoreMenuWidget> StoreMenu;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD)
	TObjectPtr<class USTMissionWidget> Mission;

	// TODO: MapWidget 추가하기, MiniMap, WorldMap

	// TODO: 현상금 게시판 추가하기
// Stat Section
public:
	void UpdateStat();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD)
	TObjectPtr<class USTStatWidget> Stat;

// InventoryMenu Section
public:
	void ShowInventoryMenu(const TArray<FSTItemSlot>& InItemSlots);
	void UpdateInventoryMenu(const TArray<FSTItemSlot>& InItemSlots);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	TSubclassOf<class USTInventoryMenuWidget> InventoryMenuClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD)
	TObjectPtr<class USTInventoryMenuWidget> InventoryMenu;

// QuickSlot Section
public:
	class USTQuickSlotWidget* GetQuickSlotWidget(int32 Index);

	void SetupQuickSlots(int32 QuickSlotCount);
	void UpdateQuickSlots(const TArray<FSTItemSlot>& InItemSlots, int32 CurrentSelectedIndex);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	TSubclassOf<class USTQuickSlotWidget> QuickSlotWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD)
	TObjectPtr<class UWrapBox> QuickSlotWrapBox;
};
