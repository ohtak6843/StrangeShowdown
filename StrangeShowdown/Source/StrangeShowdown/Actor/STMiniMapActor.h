// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "STMiniMapActor.generated.h"

UCLASS()
class STRANGESHOWDOWN_API ASTMiniMapActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASTMiniMapActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 아이템이 새로 생기면 호출해줘야 함
	void CollectItems();
	void BringHUD();

	FVector2D WorldToMiniMap(const FVector& WorldLocation) const;

	void UpdateItemOnMiniMap(float DeltaTime);
	void UpdateMiniMapRotation(float DeltaTime);

	void HiddenWidgetComponent();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MiniMap")
	class USceneCaptureComponent2D* MiniMapCapture;

	UPROPERTY(BlueprintReadOnly)
	TArray<class ASTPickupItem*> MiniMapItems;

	UPROPERTY(BlueprintReadOnly)
	class USTMiniMapWidget* MiniMapWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MiniMap")
	double zPosition = 2000.0f;
};
