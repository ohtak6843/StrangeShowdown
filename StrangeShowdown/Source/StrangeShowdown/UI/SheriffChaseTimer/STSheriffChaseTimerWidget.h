// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "STSheriffChaseTimerWidget.generated.h"

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API USTSheriffChaseTimerWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void SetTimerWidgetLocation(FVector2D InLocation);
	void SetTimerWidgetProgress(float InCurrentTime, float InMaxTime);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD, meta = (BindWidget))
	TObjectPtr<class UImage> TimerImage;

	UPROPERTY(BlueprintReadWrite)
	bool bIsTimerActive = false;

	UPROPERTY(BlueprintReadWrite)
	float HeartbeatCurrentTime = 0.f;

	UPROPERTY(BlueprintReadWrite)
	float HeartbeatMaxTime = 0.f;

private:
	float CurrentTime = 0.f;
	float MaxTime = 0.f;

	UMaterialInstanceDynamic* TimerMID;
};
