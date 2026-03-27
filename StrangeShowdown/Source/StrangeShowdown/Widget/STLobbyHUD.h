// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget/STLobbyStatusWidget.h"
#include "STLobbyHUD.generated.h"

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API USTLobbyHUD : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USTLobbyStatusWidget* LobbyStatusWidget;
};
