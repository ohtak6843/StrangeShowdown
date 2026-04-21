// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Player/STPlayerBase.h"
#include "Interface/STCharacterHUDInterface.h"
#include "InputActionValue.h"
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

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Tick(float DeltaTime) override;

	// Spring Arm Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<USpringArmComponent> SpringArmComp;

	// Camera Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> CameraComp;

// HUD Section
protected:
	virtual void SetupHUDWidget(class USTHUDWidget* InHUDWidget) override;

// Input Section
protected:
	void ShoulderMove(const FInputActionValue& Value);
	void ShoulderLook(const FInputActionValue& Value);

	// Input Mapping Context
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<class UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ShoulderMoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ShoulderLookAction;

protected:
	void SetCameraPose(ECameraPose NewPose);

	UFUNCTION(BlueprintCallable)
	void ApplyStateSettings(ECameraPose NewState);

	TObjectPtr<class UCameraComponent> GetCameraComp() { return CameraComp; }

	// Change Camera Settings with State
	void ChangeToIdle();
	void ChangeToAiming();
	void ChangeToLookingUp();

	void AddPlayerInWidget(uint64 PlayerID, const FString& NickName, bool bReady);
	void RemovePlayerFromWidget(uint64 PlayerID);

	TMap<ECameraPose, FCameraPoseSetting> PoseSettings;

	float PoseBlendTime = 0.2f;
	float PoseElapsedTime = 0.f;

	FCameraPoseSetting StartPose;
	FCameraPoseSetting TargetPose;

	TObjectPtr<class USTLobbyHUD> LobbyHUDWidget;

// Network Section
protected:
	void SendMovePacket(const float DeltaTime);

	float SendMoveDeltaTime{};
	const float SendMoveMaxTime{ 0.1f };

};
