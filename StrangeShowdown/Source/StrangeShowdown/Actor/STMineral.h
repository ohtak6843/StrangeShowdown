// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actor/STSliceableActor.h"
#include "Interface/STMiniMapTargetInterface.h"
#include "STMineral.generated.h"

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API ASTMineral : public ASTSliceableActor, public ISTMiniMapTargetInterface
{
	GENERATED_BODY()

public:
	ASTMineral();

protected:
	virtual void BeginPlay() override;
	
public:
	virtual void Slice(const FVector& HitLocation, const FVector& HitNormal, class ASTPlayerBase* Player) override;

	// BP 연출
	UFUNCTION(BlueprintImplementableEvent)
	void OnSubSlicedBlueprint();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Slice")
	UStaticMeshComponent* SubMineralMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Slice")
	USkeletalMeshComponent* SkeletalMeshComponent;

	UPROPERTY()
	APlayerCameraManager* CachedCameraManager = nullptr;

	// 미니맵에 보이는 아이콘
	TObjectPtr<UTexture2D> MiniMapIcon;

private:
	bool bIsSubSliced;

	UAnimSequence* IdleAnimation;

public:
	// ISTMiniMapTargetInterface 구현
	virtual FVector GetMiniMapLocation_Implementation() const;
	virtual UTexture2D* GetMiniMapIcon_Implementation() const;
};
