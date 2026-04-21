// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameData/STSlotData.h"
#include "GameData/STTypes.h"
#include "STHUDWidget.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSTHUDWidget, Log, All);

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

	void SetWidgetType(EHUDWidgetType InWidgetType);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD)
	EHUDWidgetType HUDWidgetType;

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
	void SetInventoryComponent(class USTInventoryComponent* InInventoryComp);

	bool OpenInventoryMenu();
	bool CloseInventoryMenu();
	void UpdateInventoryMenu();

protected:
	TWeakObjectPtr<class USTInventoryComponent> SourceInventoryComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	TSubclassOf<class USTInventoryMenuWidget> InventoryMenuClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD)
	TObjectPtr<class USTInventoryMenuWidget> InventoryMenuWidget;
#pragma endregion

#pragma region QuickSlot Section
public:
	class USTQuickSlotWidget* GetQuickSlotWidget(int32 Index);
	void SetQuickSlotComponent(class USTQuickSlotComponent* InQuickSlotComp);

	void SetupQuickSlots(int32 QuickSlotCount);
	void UpdateQuickSlots();

protected:
	TWeakObjectPtr<class USTQuickSlotComponent> SourceQuickSlotComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	TSubclassOf<class USTQuickSlotWidget> QuickSlotWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD, meta = (BindWidget))
	TObjectPtr<class UWrapBox> QuickSlotWrapBox;
#pragma endregion

#pragma region Store Section
public:
	void SetStoreComponent(class USTStoreComponent* InStoreComp);

	bool OpenStoreMenu();
	bool CloseStoreMenu();
	void UpdateStoreMenu();

protected:
	TWeakObjectPtr<class USTStoreComponent> SourceStoreComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	TSubclassOf<class USTStoreMenuWidget> StoreMenuClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD)
	TObjectPtr<class USTStoreMenuWidget> StoreMenuWidget;

#pragma endregion

#pragma region Chat Section
public:
	class USTChatManagerWidget* GetChatManagerWidget() const { return ChatManagerWidget; }

	void FocusChatManager();
	void UpdateChatManager();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD, meta = (BindWidget))
	TObjectPtr<class USTChatManagerWidget> ChatManagerWidget;

#pragma endregion

#pragma region Bounty Board Section
public:

protected:

#pragma endregion

#pragma region Timer
public:
	class USTTimerWidget* GetTimerWidget() const { return TimerWidget; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD, meta = (BindWidget))
	TObjectPtr<class USTTimerWidget> TimerWidget;

#pragma endregion

#pragma region SheriffTimer
public:
	class USTSheriffTimer* GetSheriffTimerWidget() const { return SheriffTimerWidget; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD, meta = (BindWidget))
	TObjectPtr<class USTSheriffTimer> SheriffTimerWidget;
#pragma endregion

#pragma region Area Volume Section
public:
	class USTAreaVolumeWidget* GetAreaVolumeWidget() const { return AreaVolumeWidget; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD, meta = (BindWidget))
	TObjectPtr<class USTAreaVolumeWidget> AreaVolumeWidget;

#pragma endregion

#pragma region Map Section
public:
	class USTMiniMapWidget* GetMiniMapWidget() const { return MiniMapWidget; }
	class USTMiniMapWidget* GetBigMapWidget() const { return BigMapWidget; }

	bool OpenBigMap();
	bool CloseBigMap();
	void UpdateMiniMap();
	void UpdateBigMap();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD, meta = (BindWidget))
	TObjectPtr<class USTMiniMapWidget> MiniMapWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD, meta = (BindWidget))
	TObjectPtr<class USTMiniMapWidget> BigMapWidget;
#pragma endregion

#pragma region Bounty Poster Section
public:
	bool OpenBountyPoster();
	bool CloseBountyPoster();
	void UpdateBountyPoster();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = HUD)
	TSubclassOf<class USTBountyPosterMenuWidget> BountyPosterMenuClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD)
	TObjectPtr<class USTBountyPosterMenuWidget> BountyPosterMenuWidget;
};
