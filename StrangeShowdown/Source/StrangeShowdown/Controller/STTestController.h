// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "STTestController.generated.h"

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API ASTTestController : public APlayerController
{
	GENERATED_BODY()

public:
	ASTTestController();

	virtual void SetupInputComponent() override;

protected:
	virtual void BeginPlay() override;

	void Interact();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	TSubclassOf<class USTHUDWidget> HUDWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD)
	TObjectPtr<class USTHUDWidget> HUDWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> InteractAction;
};
