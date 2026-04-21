// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item/STPickupItem.h"
#include "STBigMapActor.generated.h"

UCLASS()
class STRANGESHOWDOWN_API ASTBigMapActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASTBigMapActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void BringHUD();
	void RegisterBigMapTarget(AActor* Actor);

	// 아이템이 파괴될 때 호출
	UFUNCTION()
	void OnIconDestroyed(AActor* DestroyedActor);

	// 미니맵 좌표로 변환
	FVector2D WorldToMiniMap(const FVector& WorldLocation) const;

	// 미니맵 아이템 위치 업데이트
	void UpdateTargetOnBigMap(float DeltaTime);

	// 플레이어 아이콘 위치 업데이트
	void UpdatePlayerOnBigMap(float DeltaTime);

	void InitWidgetComponent();
	void ApplyBigMapHidden();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BigMap)
	TObjectPtr<USceneCaptureComponent2D> BigMapCapture;

	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> BigMapTargets;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class USTMiniMapWidget> BigMapWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BigMap)
	double zPosition = 2000.0f;
};
