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

	virtual void SetupInputComponent() override;

	UPROPERTY(BlueprintReadWrite, Category = "UI")
	TObjectPtr<USTHUD> HUDWidget;

	

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	// 상호작용
	virtual void Interact();

	bool IsInteracting = false;

private:
	void AddInputAction();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> InteractAction;
};
