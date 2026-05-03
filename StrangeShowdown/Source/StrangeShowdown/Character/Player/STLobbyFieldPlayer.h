// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Player/STPlayerBase.h"
#include "Components/WidgetComponent.h"
#include "InputActionValue.h"
#include "STLobbyFieldPlayer.generated.h"

/**
 * 
 */

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnFieldPlayerSpawned, uint64, const FString&, bool);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnFieldPlayerRemoved, uint64);

UCLASS()
class STRANGESHOWDOWN_API ASTLobbyFieldPlayer : public ASTPlayerBase
{
	GENERATED_BODY()

public:
	ASTLobbyFieldPlayer();

protected:
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI")
	TObjectPtr<UWidgetComponent> StatWidgetComponent;

	static FOnFieldPlayerSpawned OnFieldPlayerSpawned;
	static FOnFieldPlayerRemoved OnFieldPlayerRemoved;

// Network Section
public:
	void Init(const uint64 InPlayerID);

private:
	uint64 PlayerID{};
	const float MoveSpeed{ 12.f };
	const float RotationSpeed{ 12.f };

	UPROPERTY()
	APlayerCameraManager* CachedCameraManager = nullptr;
};