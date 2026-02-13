// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Component/STStatComponent.h"
#include "StrangeShowdown.h"
#include "STPlayerBase.generated.h"

UENUM(BlueprintType)
enum class EPlayerMeshType : uint8
{
	Badguy			UMETA(DisplayName = "Badguy"),
	BuisinessMan	UMETA(DisplayName = "BuisinessMan"),
	Cowboy			UMETA(DisplayName = "Cowboy"),
	Cowgirl			UMETA(DisplayName = "Cowgirl"),
	Gunman			UMETA(DisplayName = "Gunman"),
	Sheriff			UMETA(DisplayName = "Sheriff"),
	Woman			UMETA(DisplayName = "Woman"),
	WorkingGirl		UMETA(DisplayName = "WorkingGirl")
};


UENUM(meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EPlayerStateFlag : uint8
{
	None = Common::PlayerState::None			UMETA(DisplayName = "None"),
	Idle = Common::PlayerState::Idle			UMETA(DisplayName = "Idle"),
	Jumping = Common::PlayerState::Jumping		UMETA(DisplayName = "Jumping"),
	HoldItem = Common::PlayerState::HoldItem	UMETA(DisplayName = "HoldItem"),
	ArmedPistol = Common::PlayerState::ArmedPistol	UMETA(DisplayName = "ArmedPistol"),
	ArmedHammer = Common::PlayerState::ArmedHammer	UMETA(DisplayName = "ArmedHammer"),
	Aiming = Common::PlayerState::Aiming		UMETA(DisplayName = "Aiming"),
	LookingUp = Common::PlayerState::LookingUp	UMETA(DisplayName = "LookingUp"),
	Dead = Common::PlayerState::Dead			UMETA(DisplayName = "Dead"),
};
ENUM_CLASS_FLAGS(EPlayerStateFlag)


UCLASS()
class STRANGESHOWDOWN_API ASTPlayerBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASTPlayerBase();

	FORCEINLINE void AddState(EPlayerStateFlag NewState) { PlayerStateFlag |= static_cast<uint8>(NewState); }
	FORCEINLINE void RemoveState(EPlayerStateFlag RemoveState) { PlayerStateFlag &= ~static_cast<uint8>(RemoveState); }
	FORCEINLINE bool HasAnyState(EPlayerStateFlag CheckState) const { return (PlayerStateFlag & static_cast<uint8>(CheckState)) != 0; }
	FORCEINLINE bool HasAllState(EPlayerStateFlag CheckState) const { return (PlayerStateFlag & static_cast<uint8>(CheckState)) == static_cast<uint8>(CheckState); }

	UFUNCTION(BlueprintPure)
	bool HasAnyStateMask(UPARAM(meta = (Bitmask, BitmaskEnum = "EPlayerStateFlag")) int32 CheckState) const { return HasAnyState(static_cast<EPlayerStateFlag>(CheckState)); }

	UFUNCTION(BlueprintPure)
	bool HasAllStateMask(UPARAM(meta = (Bitmask, BitmaskEnum = "EPlayerStateFlag")) int32 CheckState) const { return HasAllState(static_cast<EPlayerStateFlag>(CheckState)); }

	UFUNCTION(BlueprintCallable, Category = "Stats")
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

protected:
	virtual void BeginPlay() override;

public:
	// Stat Component
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stats")
	TObjectPtr<USTStatComponent> StatComp;

	// Player Mesh Type
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	EPlayerMeshType PlayerMeshType;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "State", meta = (Bitmask, BitmaskEnum = "EPlayerStateFlag"))
	uint8 PlayerStateFlag;

public:
	// Character Name
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Info")
	FString PlayerNickName = TEXT("NoName");

	// Character State
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "State", Meta = (AllowPrivateAccess = "true"))
	uint8 bIsArmedPistol : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "State", Meta = (AllowPrivateAccess = "true"))
	uint8 bIsAiming : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "State", Meta = (AllowPrivateAccess = "true"))
	uint8 bIsLookingUp : 1;
};
