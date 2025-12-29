// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/STPlayerBase.h"
#include "Components/WidgetComponent.h"
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

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI")
	UWidgetComponent* StatWidgetComponent;
	
};
