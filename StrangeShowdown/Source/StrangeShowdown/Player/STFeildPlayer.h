// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/STPlayerBase.h"
#include "Components/WidgetComponent.h"
#include "STFeildPlayer.generated.h"

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API ASTFeildPlayer : public ASTPlayerBase
{
	GENERATED_BODY()

public:
	ASTFeildPlayer();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI")
	UWidgetComponent* StatWidgetComponent;
	
};
