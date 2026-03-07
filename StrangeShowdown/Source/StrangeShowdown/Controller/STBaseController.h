// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Widget/STHUD.h"
#include "STBaseController.generated.h"

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API ASTBaseController : public APlayerController
{
	GENERATED_BODY()

public:
	ASTBaseController();

	UPROPERTY(BlueprintReadWrite, Category = "UI")
	TObjectPtr<USTHUD> HUDWidget;

protected:
	virtual void BeginPlay() override;

	// 상호작용
	virtual void Interact() {};

private:
	
};
