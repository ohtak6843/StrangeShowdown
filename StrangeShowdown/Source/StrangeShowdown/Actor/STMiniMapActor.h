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

	void CollectItems();

	FVector2D WorldToMiniMap(const FVector& WorldLocation) const;

	void UpdateItemOnMiniMap(float DeltaTime);
	void UpdateMiniMapRotation(float DeltaTime);

	void HiddenWidgetComponent();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MiniMap")
	class USceneCaptureComponent2D* MiniMapCapture;

	UPROPERTY(BlueprintReadOnly)
	TArray<class ASTPickupItem*> MiniMapItems;

	// 위젯 컴포넌트가 HUD에 있으므로 나중에 연결을 하던가 해야 함
	// STMiniMapWidget에 있는 함수 2개도 완성해야 함
	UPROPERTY(BlueprintReadOnly)
	class USTMiniMapWidget* MiniMapWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MiniMap")
	double zPosition = 2000.0f;
};
