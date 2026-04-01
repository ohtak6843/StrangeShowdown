// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget/STMiniMapItemIconWidget.h"
#include "STMiniMapWidget.generated.h"

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API USTMiniMapWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void UpdateTargetIcon(class AActor* Target, const FVector2D& MiniMapPos);
	void HideTargetIcon(class AActor* Target);

	// BigMap¿ë ÇÔ¼ö
	void UpdatePlayerIcon(const FVector2D& MiniMapPos);

	void SetIsRotationAble(bool RotationAble) { IsRotationAble = RotationAble; }

public:
	UPROPERTY(meta = (BindWidget))
	class UImage* PlayerTexture;

	UPROPERTY()
	TMap<TWeakObjectPtr<AActor>, UUserWidget*> IconMap;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mini Map")
	TSubclassOf<class USTMiniMapItemIconWidget> IconClass;

	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* IconLayer;

private:
	bool IsRotationAble = true;
	
	// Ä³½Ì
	APlayerController* PlayerController;
	APawn* PlayerPawn;
	USkeletalMeshComponent* PlayerMesh;
};
