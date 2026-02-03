// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "STSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API USTSaveGame : public USaveGame
{
	GENERATED_BODY()
	
	
public:
	// »ç¿îµå
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "SaveData")
	float MasterVolume = 0.5f;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "SaveData")
	float BGMVolume = 0.5f;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "SaveData")
	float SFXVolume = 0.5f;

};
