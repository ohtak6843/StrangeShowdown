// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actor/STSliceableActor.h"
#include "STMineral.generated.h"

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API ASTMineral : public ASTSliceableActor
{
	GENERATED_BODY()

public:
	ASTMineral();

protected:
	virtual void BeginPlay() override;
	
public:
	virtual void Slice(const FVector& HitLocation, const FVector& HitNormal, class ASTPlayerBase* Player) override;

	// BP ø¨√‚
	UFUNCTION(BlueprintImplementableEvent)
	void OnSubSlicedBlueprint();

public:
	bool bIsSubSliced;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Slice")
	UStaticMeshComponent* SubMineralMeshComponent;
};
