// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/ListView.h"
#include "STMissionWidget.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FMissionInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	class UTextBlock* MissionText;

	UPROPERTY(BlueprintReadWrite)
	class UTextBlock* MissionTitle;
};

UCLASS()
class STRANGESHOWDOWN_API USTMissionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	// MissionComponent에서 호출될 함수
	UFUNCTION()
	void AddMission(const FText& NewTitle, const FText& NewMission);

protected:
	// UMG에서 BindWidget 체크
	UPROPERTY(meta = (BindWidget))
	UListView* MissionListView;

private:
	// 현재 저장된 미션 데이터
	UPROPERTY()
	TArray<UObject*> MissionItems;

	// 최대 미션 개수
	static constexpr int32 MaxMissionCount = 3;
};
