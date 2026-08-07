// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Controller/STBaseController.h"
#include "InputActionValue.h"
#include "Interface/STControllerHUDInterface.h"
#include "STGhostController.generated.h"

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API ASTGhostController : public ASTBaseController, public ISTControllerHUDInterface
{
	GENERATED_BODY()
	
public:
	ASTGhostController();

	virtual void Destroyed() override;

protected:
	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;

public:
	virtual void Init() override;

public:
	virtual class USTMiniMapWidget* GetMiniMapWidget() override;
	virtual class USTMiniMapWidget* GetBigMapWidget() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	TSubclassOf<class USTHUDWidget> HUDWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD)
	TObjectPtr<class USTHUDWidget> HUDWidget;

// Input Section
protected:
	void FocusChatManager(const FInputActionValue& Value);
	void OpenBigMap(const FInputActionValue& Value);
	void OpenBountyPoster(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<class UInputAction> FocusChatManagerAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<class UInputAction> OpenBigMapAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<class UInputAction> OpenBountyPosterAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bIsBigMapOpen : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bIsBountyPosterOpen : 1;
};
