// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item/STPickupItem.h"
#include "STMiniMapActor.generated.h"

UCLASS()
class STRANGESHOWDOWN_API ASTMiniMapActor : public AActor
{
	GENERATED_BODY()
	
public:
	ASTMiniMapActor();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	void BringHUD();
	void RegisterItem(class ASTPickupItem* NewItem);
	void RegisterMineral(class ASTMineral* NewMineral);

	// 아이템이 파괴될 때 호출
	UFUNCTION()
	void OnItemDestroyed(AActor* DestroyedActor);

	// 광물이 파괴될 때 호출
	UFUNCTION()
	void OnMineralDestroyed(AActor* DestroyedActor);

	// 미니맵 좌표로 변환
	FVector2D WorldToMiniMap(const FVector& ItemLocation, const FVector& PlayerLocation, float PlayerYaw) const;

	// 아이콘 위치 업데이트
	void UpdateItemOnMiniMap(float DeltaTime);
	void UpdateMineralOnMiniMap(float DeltaTime);
	void UpdateMiniMapRotation(float DeltaTime);

	void InitWidgetComponent();
	void ApplyMiniMapHidden();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MiniMap")
	class USceneCaptureComponent2D* MiniMapCapture;

	UPROPERTY(BlueprintReadOnly)
	TArray<class ASTPickupItem*> MiniMapItems;

	UPROPERTY(BlueprintReadOnly)
	TArray<class ASTMineral*> MiniMapMinerals;

	UPROPERTY(BlueprintReadOnly)
	class USTMiniMapWidget* MiniMapWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MiniMap")
	double zPosition = 2000.0f;

	UPROPERTY(BlueprintReadOnly)
	class USTHUD* HUDWidget;
};
