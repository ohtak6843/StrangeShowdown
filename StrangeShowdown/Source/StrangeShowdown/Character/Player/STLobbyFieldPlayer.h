// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Player/STPlayerBase.h"
#include "Components/WidgetComponent.h"
#include "STLobbyFieldPlayer.generated.h"

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API ASTLobbyFieldPlayer : public ASTPlayerBase
{
	GENERATED_BODY()
	
public:
	ASTLobbyFieldPlayer();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void BeginPlay() override;

	void Move(const FVector& Location, const FRotator& Rotator);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI")
	UWidgetComponent* StatWidgetComponent;

private:
	FVector TargetLocation{};
	FRotator TargetRotation{};
	const float SendMoveMaxTime{ 0.1f };
	const float MoveSpeed{ 12.f };
	const float RotationSpeed{ 12.f };

	UPROPERTY()
	APlayerCameraManager* CachedCameraManager = nullptr;
};
