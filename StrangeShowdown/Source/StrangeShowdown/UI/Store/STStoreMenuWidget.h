// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "STStoreMenuWidget.generated.h"

DECLARE_DELEGATE_RetVal(bool, FOnExitButtonClicked);

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API USTStoreMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	USTStoreMenuWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

	void HandleExitButtonHovered();
	void HandleExitButtonUnhovered();
	void HandleExitButtonClicked();

public:
	FOnExitButtonClicked OnExitButtonClicked;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class USTStoreWidget> StoreWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> ExitButton;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class USoundBase> HoverSound;

	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	TObjectPtr<class UWidgetAnimation> ExitHoverAnimation;
};
