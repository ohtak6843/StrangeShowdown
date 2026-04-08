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
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 현재 추적 중인 플레이어
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AttackTrace|State")
	TObjectPtr<ASTFieldPlayer> TracingFieldPlayer;

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
