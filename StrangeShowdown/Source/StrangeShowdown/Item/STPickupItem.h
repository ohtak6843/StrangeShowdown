// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item/STItemDataAssetBase.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Character/Player/STLocalPlayer.h"
#include "STPickupItem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerEnter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerExit);

UCLASS()
class STRANGESHOWDOWN_API ASTPickupItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASTPickupItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void OnConstruction(const FTransform& Transform) override;

private:
	UFUNCTION()
	void HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void HandleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	// 월드에서 보여지는 메쉬
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item")
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item")
	UStaticMeshComponent* SubMesh;

	// 콜리전
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item")
	USphereComponent* PickupCollision;

	// Overlap 이벤트
	UPROPERTY(BlueprintAssignable)
	FOnPlayerEnter OnPlayerEnter;
	UPROPERTY(BlueprintAssignable)
	FOnPlayerExit OnPlayerExit;

	// 아이템
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	USTItemDataAssetBase* ItemData;

	// Picup UI 위젯
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI")
	UWidgetComponent* PickupWidgetComponent;

	// 아이템과 오버랩 중인 플레이어 저장
	UPROPERTY()
	ASTLocalPlayer* OverlappedPlayer = nullptr;

	UPROPERTY()
	APlayerCameraManager* CachedCameraManager = nullptr;
};