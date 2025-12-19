// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "STStatComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STRANGESHOWDOWN_API USTStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USTStatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void AddHp(int32 HealAmount);

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void AddGold(int32 GoldAmount);

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void AddKill(int32 KillAmount);

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void AddMoveSpeed(int32 MoveSpeedAmount);

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void AddStamina(int32 StaminaAmount);

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void AddAction(int32 ActionAmount);

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void AddPrize(int32 PrizeAmount);

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void SetIsActive(bool isActive);

	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float Hp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float MaxHp = 15;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	int32 Gold;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	int32 Kill;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float MoveSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float Stamina;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float MaxStamina = 5;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	int32 Action;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	int32 MaxAction = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	int32 Prize;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	bool bAlive;
		
};
