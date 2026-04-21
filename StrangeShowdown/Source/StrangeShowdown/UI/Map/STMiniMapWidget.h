// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/Map/STMiniMapItemIconWidget.h"
#include "STMiniMapWidget.generated.h"

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API USTMiniMapWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	USTMiniMapWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	class UImage* GetRenderTargetTexture() const { return RenderTargetTexture; }

	void UpdateTargetIcon(class AActor* Target, const FVector2D& MiniMapPos);
	void HideTargetIcon(class AActor* Target);

	// BigMap¿ë ÇÔ¼ö
	void UpdatePlayerIcon(const FVector2D& MiniMapPos);

	void SetIsRotationAble(bool RotationAble) { IsRotationAble = RotationAble; }

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Map, meta = (BindWidget))
	TObjectPtr<class UImage> PlayerTexture;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Map, meta = (BindWidget))
	TObjectPtr<class UImage> RenderTargetTexture;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Map)
	TSubclassOf<class USTMiniMapItemIconWidget> IconClass;

	UPROPERTY()
	TMap<TWeakObjectPtr<AActor>, UUserWidget*> IconMap;

	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* IconLayer;

private:
	bool IsRotationAble = true;
	
	// Ä³½Ì
	TWeakObjectPtr<class APlayerController> PlayerController;
	TWeakObjectPtr<class APawn> PlayerPawn;
	TWeakObjectPtr<class USkeletalMeshComponent> PlayerMesh;
};
