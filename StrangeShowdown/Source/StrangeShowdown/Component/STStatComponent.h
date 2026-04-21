// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "STStatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnStatChanged);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STRANGESHOWDOWN_API USTStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USTStatComponent();

public:
	FOnStatChanged OnStatChanged;

public:
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void AddHp(int32 HealAmount);

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void AddGold(int32 GoldAmount);

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void AddKill(int32 KillAmount);

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void AddArmor(int32 ArmorAmount);

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void AddMoveSpeed(int32 MoveSpeedAmount);

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void AddStamina(int32 StaminaAmount);

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void AddAction(int32 ActionAmount);

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void AddUseAbleAction(int32 ActionAmount);

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void AddPrize(int32 PrizeAmount);

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void SetIsActive(bool isActive);

	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stats")
	float CurrentHp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxHp = 15;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stats")
	int32 Gold;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stats")
	int32 CurrentArmor = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stats")
	int32 MaxArmor = 30;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stats")
	int32 Kill;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stats")
	float MoveSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stats")
	float CurrentStamina;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxStamina = 5;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stats")
	int32 CurrentAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stats")
	int32 UseAbleAction = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stats")
	int32 MaxAction = 6;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stats")
	int32 Bounty;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stats")
	bool bAlive;
};
