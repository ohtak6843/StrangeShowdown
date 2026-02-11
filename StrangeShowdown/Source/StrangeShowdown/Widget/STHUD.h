// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget/STChatManager.h"
#include "STHUD.generated.h"

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API USTHUD : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USTChatManager* ChatManagerWidget;
};
