// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Player/STPlayerBase.h"
#include "Interface/STCharacterHUDInterface.h"
#include "STLobbyLocalPlayer.generated.h"

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API ASTLobbyLocalPlayer : public ASTPlayerBase, public ISTCharacterHUDInterface
{
	GENERATED_BODY()

public:
	ASTLobbyLocalPlayer();
	
	void SetCameraPose(ECameraPose NewPose);

	UFUNCTION(BlueprintCallable)
	void ApplyStateSettings(ECameraPose NewState);

	TObjectPtr<class UCameraComponent> GetCameraComp() { return CameraComp; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Tick(float DeltaTime) override;

	// Spring Arm Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<USpringArmComponent> SpringArmComp;

	// Camera Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> CameraComp;

protected:
	virtual void SetupHUDWidget(class USTHUDWidget* InHUDWidget) override;

private:
	// Change Camera Settings with State
	void ChangeToIdle();
	void ChangeToAiming();
	void ChangeToLookingUp();

	void SendMovePacket(const float DeltaTime);
	void AddPlayerInWidget(uint64 PlayerID, const FString& NickName, bool bReady);
	void RemovePlayerFromWidget(uint64 PlayerID);

	TMap<ECameraPose, FCameraPoseSetting> PoseSettings;

	float PoseBlendTime = 0.2f;
	float PoseElapsedTime = 0.f;

	FCameraPoseSetting StartPose;
	FCameraPoseSetting TargetPose;

	// Network
	float SendMoveDeltaTime{};

	TObjectPtr<class USTLobbyHUD> LobbyHUDWidget;
};
