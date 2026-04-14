// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "STTimerWidget.generated.h"

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API USTTimerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void SetTimer(float InMaxTime);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD, meta = (BindWidget))
	TObjectPtr<class UImage> TimerImage;
	
private:
	float CurrentTime = 0.f;
	float MaxTime = 0.f;

	UMaterialInstanceDynamic* TimerMID;
};
