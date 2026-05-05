// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "STControllerHUDInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USTControllerHUDInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class STRANGESHOWDOWN_API ISTControllerHUDInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual class USTChatManagerWidget* GetChatManagerWidget() { return nullptr; }
	virtual class USTMiniMapWidget* GetMiniMapWidget() { return nullptr; }
	virtual class USTMiniMapWidget* GetBigMapWidget() { return nullptr; }

	virtual void OpenStore() {}
	virtual void CloseStore() {}
};
