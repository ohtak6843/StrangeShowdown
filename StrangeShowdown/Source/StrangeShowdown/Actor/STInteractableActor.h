// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "STInteractableActor.generated.h"

UINTERFACE(Blueprintable)
class STRANGESHOWDOWN_API UInteractable : public UInterface
{
	GENERATED_BODY()
};

class STRANGESHOWDOWN_API IInteractable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Interact(APawn* Interactor);
};