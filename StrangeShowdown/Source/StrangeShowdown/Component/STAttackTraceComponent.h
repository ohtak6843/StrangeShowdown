#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "STAttackTraceComponent.generated.h"

class UWidgetComponent;
class ASTFieldPlayer;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class STRANGESHOWDOWN_API USTAttackTraceComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USTAttackTraceComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	// 델리게이트를 받아 위젯에 타겟 획득 알림, 위치 Set, 위젯 삭제
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void HandleTargetAcquired(ASTFieldPlayer* NewTarget);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void HandleTargetLost();

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
		FOnAttackTraceTargetAcquired,
		ASTFieldPlayer*, NewTarget
	);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(
		FOnAttackTraceTargetLost
	);

public:
	// UI 위젯 클래스 (설계도)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackTrace|UI")
	TSubclassOf<UUserWidget> AttackTraceWidgetClass;

	// 현재 추적 중인 플레이어
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AttackTrace|State")
	TObjectPtr<ASTFieldPlayer> TracingFieldPlayer;

	// 실제 생성된 위젯 인스턴스
	UPROPERTY(Transient)
	TObjectPtr<UWidgetComponent> AttackTraceWidgetComponent;

public:
	// 타겟 획득 시
	UPROPERTY(BlueprintAssignable, Category = "AttackTrace|Event")
	FOnAttackTraceTargetAcquired OnTargetAcquired;

	// 타겟 상실 시
	UPROPERTY(BlueprintAssignable, Category = "AttackTrace|Event")
	FOnAttackTraceTargetLost OnTargetLost;

	// 공격 추적 시작 여부
	UPROPERTY(BlueprintReadWrite, Category = "AttackTrace|Bool")
	bool bStartAttackTrace;
	
	// 추적 사거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackTrace|Config")
	float TraceDistance = 1500.f;

private:
	// 타겟 변경 처리
	void SetTracingTarget(ASTFieldPlayer* NewTarget);

	// 시야 검사
	ASTFieldPlayer* FindTargetInSight() const;
};
