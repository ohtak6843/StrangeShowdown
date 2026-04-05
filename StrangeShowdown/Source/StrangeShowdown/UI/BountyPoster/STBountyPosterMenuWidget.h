// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "STBountyPosterMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API USTBountyPosterMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	USTBountyPosterMenuWidget(const FObjectInitializer& ObjectInitializer);

	void UpdateBountyPosterMenu();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class USTBountyPosterWidget> BountyPosterWidget;	
};
