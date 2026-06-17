// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Player/STPlayerBase.h"
#include "Components/WidgetComponent.h"
#include "Component/STTargetIndicatorComponent.h"
#include "Manager/STDataManager.h"
#include "STFieldPlayer.generated.h"

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API ASTFieldPlayer : public ASTPlayerBase
{
	GENERATED_BODY()

public:
	ASTFieldPlayer();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

// Dead Section
public:
	virtual void ChangeToGhost() override;

// Effect Section
public:
	UFUNCTION(BlueprintImplementableEvent)
	void PlayPistolFireEffect();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI")
	TObjectPtr<class USTWidgetComponent> StatWidgetComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI")
	TObjectPtr<USTTargetIndicatorComponent> TargetIndicatorComponent;

	void Init(const FPlayerInfo& PlayerInfo);

// getters
public:
	uint64 GetPlayerID() const { return PlayerID; }

private:

	uint64 PlayerID{};

	UPROPERTY()
	APlayerCameraManager* CachedCameraManager = nullptr;

	const float MoveSpeed{ 12.f };
	const float RotationSpeed{ 12.f };
};
