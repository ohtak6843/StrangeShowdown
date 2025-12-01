// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "STItemDataAssetBase.h"
#include "STPickupItemBase.generated.h"

UCLASS()
class STRANGESHOWDOWN_API ASTPickupItemBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASTPickupItemBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void OnConstruction(const FTransform& Transform) override;

public:
	// 월드에서 보여지는 메쉬
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item")
	UStaticMeshComponent* Mesh;

	// 메쉬 위치
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	FVector MeshPos = FVector(0.f, 0.f, 0.f);

	// 콜리전
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item")
	USphereComponent* PickupCollision;

	// 아이템
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	USTItemDataAssetBase* ItemData;
};