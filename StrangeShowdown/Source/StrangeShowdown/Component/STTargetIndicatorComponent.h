// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/WidgetComponent.h"
#include "STTargetIndicatorComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STRANGESHOWDOWN_API USTTargetIndicatorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USTTargetIndicatorComponent();

protected:
	// Called when the game starts
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	void AddAttacker(class USTAttackTraceComponent* Attacker);
	void RemoveAttacker(class USTAttackTraceComponent* Attacker);
	void UpdateUI();

	// UI 위젯 클래스 (설계도)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackTrace|UI")
	TSubclassOf<UUserWidget> AttackTraceWidgetClass;

	// 실제 생성된 위젯 인스턴스
	UPROPERTY(Transient)
	TObjectPtr<UWidgetComponent> AttackTraceWidgetComponent;
		
	// spine03으로부터 위젯 생성 위치
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackTrace|UI")
	FVector WidgetOffset = FVector(0.f, 0.f, 0.f);

private:
	TSet<TWeakObjectPtr<USTAttackTraceComponent>> Attackers;
};
