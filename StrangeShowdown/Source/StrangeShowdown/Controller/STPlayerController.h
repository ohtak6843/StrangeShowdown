// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Controller/STBaseController.h"
#include "InputActionValue.h"
#include "Interface/STControllerHUDInterface.h"
#include "STPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API ASTPlayerController : public ASTBaseController, public ISTControllerHUDInterface
{
	GENERATED_BODY()

public:
	ASTPlayerController();

protected:
	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;

public:
	class USTHUDWidget* GetHUDWidget() const { return HUDWidget; }
	virtual class USTMiniMapWidget* GetMiniMapWidget() override;
	virtual class USTMiniMapWidget* GetBigMapWidget() override;

	void OpenStore();

	UFUNCTION(BlueprintImplementableEvent, Category = "Store")
	void CreateStoreWidget();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	TSubclassOf<class USTHUDWidget> HUDWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD)
	TObjectPtr<class USTHUDWidget> HUDWidget;

// Input Section
protected:
	void LookingUp(const FInputActionValue& Value);
	void OpenInventory(const FInputActionValue& Value);
	void FocusChatManager(const FInputActionValue& Value);
	void OpenBigMap(const FInputActionValue& Value);
	void OpenBountyPoster(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<class UInputAction> LookingUpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<class UInputAction> OpenInventoryAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<class UInputAction> FocusChatManagerAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<class UInputAction> OpenBigMapAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<class UInputAction> OpenBountyPosterAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bIsInventoryOpen : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bIsStoreOpen : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bIsBigMapOpen : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bIsBountyPosterOpen : 1;
};
