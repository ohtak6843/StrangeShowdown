// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Player/STPlayerBase.h"
#include "GameData/STTypes.h"
#include "InputActionValue.h"
#include "GameData/STSlotData.h"
#include "Interface/STCharacterHUDInterface.h"
#include "Interface/STAnimAttackInterface.h"
#include "STLocalPlayer.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLookingUpStateActivated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLookingUpStateDeactivated);

UCLASS()
class STRANGESHOWDOWN_API ASTLocalPlayer : public ASTPlayerBase, public ISTAnimAttackInterface, public ISTCharacterHUDInterface
{
	GENERATED_BODY()
	
public:
	// Sets default values for this character's properties
	ASTLocalPlayer();

	virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay() override;

// Debug Section
public:
	UFUNCTION(BlueprintCallable, Category = Debug)
	void AddGold(int32 Amount) { StatComp->GetCharacterStat().SetCurrentGold(StatComp->GetCharacterStat().CurrentGold + Amount); StatComp->OnStatChanged.Broadcast(); }
	
	UFUNCTION(BlueprintCallable, Category = Debug)
	void AddStamina(float Amount) { StatComp->GetCharacterStat().SetCurrentStamina(StatComp->GetCharacterStat().CurrentStamina + Amount); StatComp->OnStatChanged.Broadcast(); }

// DELEGATE Section
public:
	UPROPERTY(BlueprintAssignable, Category = Delegate)
	FOnLookingUpStateActivated OnLookingUpStateActivated;
	UPROPERTY(BlueprintAssignable, Category = Delegate)
	FOnLookingUpStateDeactivated OnLookingUpStateDeactivated;

// Damage Section
public:
	virtual ASTCharacter* ChangeToGhost() override;

// HUD Section
public:
	virtual USTHUDWidget* GetHUDWidget(class USTHUDWidget* OutHUDWidget) { return HUDWidget.Get(); }

protected:
	virtual void SetupHUDWidget(class USTHUDWidget* InHUDWidget) override;

	TWeakObjectPtr<USTHUDWidget> HUDWidget;

// Message Section
	UFUNCTION(BlueprintImplementableEvent)
	void ShowFloatingMessage(const FText& Message);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = HUD)
	TSoftObjectPtr<class USoundBase> BuySound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = HUD)
	TSoftObjectPtr<class USoundBase> ErrorSound;

// Attack Hit Interface Section
public:
	virtual void AttackHitCheck() override;

// Blueprint Function Section
public:
	UFUNCTION(BlueprintCallable, Category = "Item")
	bool UseItem();

	UFUNCTION(BlueprintCallable)
	void HoldItem();

	UFUNCTION(BlueprintImplementableEvent)
	void PistolFire();

	UFUNCTION(BlueprintImplementableEvent)
	void HammerSmash();

	virtual void HandleStoreSlotClicked(const struct FStoreSlot& InStoreSlot) override; 

	TObjectPtr<class UCameraComponent> GetCameraComp() { return CameraComp; }

	TObjectPtr<class USTStoreComponent> GetStoreComp() { return StoreComp; }

	TObjectPtr<class USTMissionComponent> GetMissionComp() { return MissionComponent; }

protected:
	// Spring Arm Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<USpringArmComponent> SpringArmComp;

	// Camera Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> CameraComp;

	// Inventory Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<class USTInventoryComponent> InventoryComp;

	// QuickSlot Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "QuickSlot")
	TObjectPtr<class USTQuickSlotComponent> QuickSlotComp;

	// Store Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Store")
	TObjectPtr<class USTStoreComponent> StoreComp;

	// Attack Trace Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AttackTrace")
	TObjectPtr<class USTAttackTraceComponent> AttackTraceComp;

	// Mission Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mission")
	TObjectPtr<class USTMissionComponent> MissionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Smash")
	bool IsSmashing = false;

// Input Section
protected:
	void ShoulderMove(const FInputActionValue& Value);
	void ShoulderLook(const FInputActionValue& Value);
	void PistolAim(const FInputActionValue& Value);
	void LookingUp(const FInputActionValue& Value);
	void UseQuickSlotItem(const FInputActionValue& Value);
	void ChangeQuickSlot(const FInputActionValue& Value);
	void ScrollQuickSlot(const FInputActionValue& Value);
	void PickUp(const FInputActionValue& Value);
	void DropItem(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<class UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<class UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<class UInputAction> ShoulderMoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<class UInputAction> ShoulderLookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<class UInputAction> PistolAimAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<class UInputAction> LookingUpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<class UInputAction> UseQuickSlotItemAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<class UInputAction> ChangeQuickSlotAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<class UInputAction> ScrollQuickSlotAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<class UInputAction> PickUpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<class UInputAction> DropItemAction;

// Montage Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Montage)
	TObjectPtr<class UAnimMontage> PistolFireMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Montage)
	TObjectPtr<class UAnimMontage> SmashMontage;

// Dead Section
public:
	virtual void SetDead() override;

// Effect Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Effect)
	TObjectPtr<class UNiagaraSystem> MuzzleFlashEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Effect)
	TObjectPtr<class UNiagaraSystem> HitEffect;

// Camera Pose Section
protected:
	void ApplyStateSettings(ECameraPose NewState);
	void SetCameraPose(ECameraPose NewPose);
	void ChangeToIdle();
	void ChangeToAiming();
	void ChangeToLookingUp();

	TMap<ECameraPose, FCameraPoseSetting> PoseSettings;

	float PoseBlendTime = 0.2f;
	float PoseElapsedTime = 0.f;

	FCameraPoseSetting StartPose;
	FCameraPoseSetting TargetPose;

// Sheriff Chase Section
public:
	void SetFieldSheriff(class ASTFieldSheriff* InSheriff);
	void ClearSheriff();
	void SheriffChaseUpdate();

	void DrawPersistentLine(FVector Start, FVector End, FColor Color, float Thickness = 2.f);
	void ClearPersistentLines();

	UFUNCTION(BlueprintCallable)
	void TestAddSheriffTransform();

protected:
	TObjectPtr<class ASTFieldSheriff> FieldSheriff;
	bool bIsChasingSheriff = false;

private:
	UPROPERTY()
	class ULineBatchComponent* LineBatcher;

// MapObject Section
public:
	void ActiveAlterFirstMapObject();
	void ActiveBankFirstMapObject();
	void ActiveChurchFirstMapObject();
	void ActiveStationFirstMapObject();
	void ActivePubFirstMapObject();

// Network Section
public:
	void Init();

};
