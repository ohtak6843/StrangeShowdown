// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Widget/STHUD.h"
#include "STPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API ASTPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(BlueprintReadWrite, Category = "UI")
	TObjectPtr<USTHUD> HUDWidget;
};
