#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Game/STMissionRowData.h"
#include "STMissionWidget.generated.h"

USTRUCT()
struct FMissionSlot
{
	GENERATED_BODY()

	UPROPERTY()
	UTextBlock* Title = nullptr;

	UPROPERTY()
	UTextBlock* Mission = nullptr;

	UPROPERTY()
	UTextBlock* MissionProgress = nullptr;

	UPROPERTY()
	UImage* Image = nullptr;
};

UCLASS()
class STRANGESHOWDOWN_API USTMissionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	// Delegate 바인딩용
	UFUNCTION()
	void AddMission(USTMissionRowData* Data);

	UFUNCTION()
	void MissionClear(USTMissionRowData* Data);

	UFUNCTION()
	void UpdateMissionProgress(USTMissionRowData* Data);

	void RemoveMission(USTMissionRowData* Data);
	void RebuildSlots();

	UFUNCTION(BlueprintNativeEvent, Category = "Mission")
	void MissionAnimation(int32 Index);

protected:
	// 슬롯 (UI)
	UPROPERTY()
	TArray<FMissionSlot> MissionSlots;

	// 데이터
	UPROPERTY(BlueprintReadOnly, Category = "Mission")
	TArray<USTMissionRowData*> ActiveMissions;

	// BindWidget
	UPROPERTY(meta = (BindWidget)) UTextBlock* Mission1Title;
	UPROPERTY(meta = (BindWidget)) UTextBlock* Mission2Title;
	UPROPERTY(meta = (BindWidget)) UTextBlock* Mission3Title;

	UPROPERTY(meta = (BindWidget)) UTextBlock* Mission1;
	UPROPERTY(meta = (BindWidget)) UTextBlock* Mission2;
	UPROPERTY(meta = (BindWidget)) UTextBlock* Mission3;

	UPROPERTY(meta = (BindWidget)) UTextBlock* Mission1Progress;
	UPROPERTY(meta = (BindWidget)) UTextBlock* Mission2Progress;
	UPROPERTY(meta = (BindWidget)) UTextBlock* Mission3Progress;

	UPROPERTY(meta = (BindWidget)) UImage* Mission1Image;
	UPROPERTY(meta = (BindWidget)) UImage* Mission2Image;
	UPROPERTY(meta = (BindWidget)) UImage* Mission3Image;
};