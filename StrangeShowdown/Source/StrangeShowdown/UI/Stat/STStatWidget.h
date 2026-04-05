// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameData/STTypes.h"
#include "STStatWidget.generated.h"

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API USTStatWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	USTStatWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

public:
	void SetStatComponent(class USTStatComponent* InStatComponent);
	void SetWidgetType(EHUDWidgetType InWidgetType);
	void SetPlayerWidget();
	void SetGhostWidget();
	void SetSheriffWidget();

	void UpdateStat();

protected:
	TWeakObjectPtr<class USTStatComponent> SourceStatComp;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UProgressBar> HpBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UProgressBar> ShieldBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UProgressBar> SheriffGaugeBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UProgressBar> GhostGaugeBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> HpText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> ShieldText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> HeartImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> GoldImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> GoldText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UWrapBox> StaminaWrapBox;

	UPROPERTY()
	TArray<TObjectPtr<class UImage>> StaminaImages;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UWrapBox> ActionWrapBox;

	UPROPERTY()
	TArray<TObjectPtr<class UImage>> ActionImages;

// Animation Section
protected:
	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	TObjectPtr<class UWidgetAnimation> PlayerHeartbeat;

	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	TObjectPtr<class UWidgetAnimation> SheriffHeartbeat;

	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	TObjectPtr<class UWidgetAnimation> GhostHeartbeat;

// Heart Image Section
protected:
	UPROPERTY()
	TObjectPtr<class UTexture2D> PlayerHeartTexture;

	UPROPERTY()
	TObjectPtr<class UTexture2D> GhostHeartTexture;

	UPROPERTY()
	TObjectPtr<class UTexture2D> SheriffHeartTexture;
};
