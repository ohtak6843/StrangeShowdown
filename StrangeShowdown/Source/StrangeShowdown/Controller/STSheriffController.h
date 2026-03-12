// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Controller/STBaseController.h"
#include "Widget/STHUD.h"
#include "STSheriffController.generated.h"

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API ASTSheriffController : public ASTBaseController
{
	GENERATED_BODY()

public:
	ASTSheriffController();

	virtual void SetupInputComponent() override;

protected:
	virtual void BeginPlay() override;

private:
	void AddInputAction();
	void ShowTabUI();
	void ShowBigMap();
	void FocusChatManager();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> TabUIAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> BigMapAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> FocusChatManagerAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, Meta = (AllowPrivateAccess = "true"))
	bool IsTabUIOpen = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, Meta = (AllowPrivateAccess = "true"))
	bool IsBigMapOpen = false;
};
