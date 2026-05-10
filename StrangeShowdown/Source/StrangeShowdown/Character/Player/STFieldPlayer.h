// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Player/STPlayerBase.h"
#include "Components/WidgetComponent.h"
#include "Component/STTargetIndicatorComponent.h"
#include "STFieldPlayer.generated.h"

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API ASTFieldPlayer : public ASTPlayerBase
{
	GENERATED_BODY()

public:
	ASTFieldPlayer();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI")
	TObjectPtr<USTWidgetComponent> StatWidgetComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI")
	TObjectPtr<USTTargetIndicatorComponent> TargetIndicatorComponent;

private:

	UPROPERTY()
	APlayerCameraManager* CachedCameraManager = nullptr;

	const float MoveSpeed{ 12.f };
	const float RotationSpeed{ 12.f };
};
