// File: Source/StrangeShowdown/Public/VictoryCamera.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VictoryCamera.generated.h"

UCLASS()
class STRANGESHOWDOWN_API AVictoryCamera : public AActor
{
    GENERATED_BODY()

public:
    AVictoryCamera();

public:
    virtual void Tick(float DeltaTime) override;

    // 승자 액터 설정 및 호 이동 시작
    UFUNCTION(BlueprintCallable, Category = "VictoryCamera")
    void StartArcSwing(AActor* InTargetActor);

private:
    // 45도 기울어진 대원 위 위치 계산
    // T: 0.0 ~ 1.0 (0~2PI 각도에 매핑)
    FVector CalcArcPosition(float T) const;

    // 카메라 컴포넌트
    UPROPERTY(VisibleAnywhere, Category = "Components")
    class UCameraComponent* CameraComponent;

    // 궤도 대상 (승자 액터)
    UPROPERTY()
    AActor* TargetActor;

    // 타겟으로부터의 거리 (cm)
    UPROPERTY(EditAnywhere, Category = "Arc Settings")
    float Distance;

    // 카메라 기본 높이 (cm)
    UPROPERTY(EditAnywhere, Category = "Arc Settings")
    float BaseHeight;

    // 뷰 전환 블렌드 시간 (초)
    UPROPERTY(EditAnywhere, Category = "Arc Settings")
    float BlendTime;

    // 각 Phase 체류 시간 (초)
    // Phase 0: LeftUp 체류, Phase 2: RightDown 체류
    UPROPERTY(EditAnywhere, Category = "Arc Settings")
    float HoldDuration;

    // Phase 간 이동 시간 (초)
    // Phase 1: LeftUp→RightDown, Phase 3: RightDown→Front
    UPROPERTY(EditAnywhere, Category = "Arc Settings")
    float TransitDuration;

    // 대원 위 위치를 결정하는 T값 (0.0 ~ 1.0)
    // T=0.0  : 왼쪽+위 (시작)
    // T=0.5  : 오른쪽+아래
    // T=0.75 : 정면 (종료)
    UPROPERTY(EditAnywhere, Category = "Arc Settings", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float ArcT_LeftUp;

    UPROPERTY(EditAnywhere, Category = "Arc Settings", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float ArcT_RightDown;

    UPROPERTY(EditAnywhere, Category = "Arc Settings", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float ArcT_Front;

    // 경과 시간 (내부용)
    float ElapsedTime;

    // 총 재생 시간 (내부용)
    float TotalDuration;

    // StartArcSwing 호출 시점의 TargetActor 정면 Yaw (고정)
    // Tick 중 TargetActor가 회전해도 카메라 기준축이 흔들리지 않도록 고정
    float BaseYaw;

    // 현재 Phase
    // 0: LeftUp 체류
    // 1: LeftUp → RightDown 이동
    // 2: RightDown 체류
    // 3: RightDown → Front 이동
    // 4: Front 체류/종료
    int32 CurrentPhase;

    // Phase 내 경과 시간
    float PhaseElapsed;

    UPROPERTY(EditAnywhere, Category = "Arc Settings", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float DriftAmount;

    // 호 이동 작동 여부
    uint8 bIsSwinging : 1;
};