// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameData/STCharacterStat.h"
#include "STStatComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStatChanged);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STRANGESHOWDOWN_API USTStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USTStatComponent();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

// Delegate Section
public:
	UPROPERTY(BlueprintAssignable)
	FOnStatChanged OnStatChanged;

public:
	FSTCharacterStat& GetCharacterStat() { return CharacterStat; }
	void SetCharacterStat(const FSTCharacterStat& InStat) { CharacterStat = InStat; }

	void SetCurrentHp(float NewHp) { CharacterStat.SetCurrentHp(NewHp); OnStatChanged.Broadcast(); }
	void SetCurrentArmor(float NewArmor) { CharacterStat.SetCurrentArmor(NewArmor); OnStatChanged.Broadcast(); }
	void SetCurrentStamina(float NewStamina) { CharacterStat.SetCurrentStamina(NewStamina); OnStatChanged.Broadcast(); }
	void SetCurrentAction(float NewAction) { CharacterStat.SetCurrentAction(NewAction); OnStatChanged.Broadcast(); }
	void SetUsableAction(float NewUsableAction) { CharacterStat.SetUsableAction(NewUsableAction); OnStatChanged.Broadcast(); }
	void SetCurrentGold(float NewGold) { CharacterStat.SetCurrentGold(NewGold); OnStatChanged.Broadcast(); }
	void SetKillCount(float NewKillCount) { CharacterStat.SetKillCount(NewKillCount); OnStatChanged.Broadcast(); }
	void SetBounty(float NewBounty) { CharacterStat.SetBounty(NewBounty); OnStatChanged.Broadcast(); }

protected:
	void ResetMoveSpeed();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Stat)
	FSTCharacterStat CharacterStat;

	FTimerHandle MoveSpeedBuffTimerHandle;

// network methods
public:
	// 게임 시작 전 플레이어의 스탯 초기화
	void InitPlayerStats();
};
