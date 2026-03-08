// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Controller/STBaseController.h"
#include "Widget/STHUD.h"
#include "STPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API ASTPlayerController : public ASTBaseController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	void Interact() override;
};
