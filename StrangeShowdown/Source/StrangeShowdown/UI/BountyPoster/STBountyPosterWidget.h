// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "STBountyPosterWidget.generated.h"

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API USTBountyPosterWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	USTBountyPosterWidget(const FObjectInitializer& ObjectInitializer);

	void UpdateBountyPoster();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BountyPoster)
	TSubclassOf<class USTBountyPosterSlotWidget> BountyPosterSlotWidgetClass;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UWrapBox> BountyPosterSlotWrapBox;
};
