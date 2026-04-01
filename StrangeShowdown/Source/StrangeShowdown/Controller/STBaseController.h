// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
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

public:
	// TODO: 나중에 어떻게 할지 정하기
	class USTHUD* GetHUDWidget() const { return nullptr; }

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void Interact();

private:
	void AddInputAction();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> InteractAction;
};
