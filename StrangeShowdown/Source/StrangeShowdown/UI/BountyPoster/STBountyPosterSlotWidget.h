// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "STBountyPosterSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API USTBountyPosterSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	USTBountyPosterSlotWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

public:
	void UpdateBountyPosterSlot(class ASTPlayerBase* InPlayer);

protected:
	UPROPERTY()
	TArray<TObjectPtr<class UImage>> StaminaImages;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> PlayerBountyText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> PlayerNameText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> PlayerImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> PlayerGold;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UProgressBar> PlayerHpBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> PlayerHpText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Texture)
	TObjectPtr<class UTexture2D> TempTexture;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Texture)
	TArray<TSoftObjectPtr<UTexture2D>> Player2DTextures;
};
