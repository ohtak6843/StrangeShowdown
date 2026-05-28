
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "STCharacterStat.generated.h"

USTRUCT(BlueprintType)
struct FSTCharacterStat : public FTableRowBase
{
	GENERATED_BODY()

public:
	FSTCharacterStat() : MaxHp(100.f), CurrentHp(100.f), MaxArmor(30.f), CurrentArmor(0.f), MaxStamina(5.f), CurrentStamina(0.f),
		CurrentAction(0.f), UsableAction(3.f), MaxAction(6.f), CurrentGold(0.f), KillCount(0.f), Bounty(0.f) {
	}

	void SetCurrentHp(float NewHp) { CurrentHp = FMath::Clamp(NewHp, 0.f, MaxHp); }
	void SetCurrentArmor(float NewArmor) { CurrentArmor = FMath::Clamp(NewArmor, 0.f, MaxArmor); }
	void SetCurrentStamina(float NewStamina) { CurrentStamina = FMath::Clamp(NewStamina, 0.f, MaxStamina); }
	void SetCurrentAction(float NewAction) { CurrentAction = FMath::Clamp(NewAction, 0.f, UsableAction); }
	void SetUsableAction(float NewUsableAction) { UsableAction = FMath::Clamp(NewUsableAction, 0.f, MaxAction); }
	void SetCurrentGold(float NewGold) { CurrentGold = NewGold; }
	void SetKillCount(float NewKillCount) { KillCount = NewKillCount; }
	void SetBounty(float NewBounty) { Bounty = NewBounty; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float MaxHp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float CurrentHp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float MaxArmor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float CurrentArmor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float MaxStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float CurrentStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float CurrentAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float UsableAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float MaxAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float CurrentGold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float KillCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float Bounty;
};
