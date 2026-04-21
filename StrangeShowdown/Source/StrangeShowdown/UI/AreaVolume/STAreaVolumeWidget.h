// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "STAreaVolumeWidget.generated.h"

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API USTAreaVolumeWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void UpdateAreaInfo(enum EAreaType AreaType);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void DeleteAreaInfo(enum EAreaType AreaType);
};
