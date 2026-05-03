// Fill out your copyright notice in the Description page of Project Settings.

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

    // 좌우 호의 최대 각도 (도)
    // 예: 60이면 왼쪽 60도 ~ 오른쪽 60도 사이를 왕복
    UPROPERTY(EditAnywhere, Category = "Arc Settings")
    float SwingAngle;

    // 높이 변화 범위 (cm)
    // 왼쪽 위일 때 +HeightVariance, 오른쪽 아래일 때 -HeightVariance
    UPROPERTY(EditAnywhere, Category = "Arc Settings")
    float HeightVariance;

    // 왕복 속도 (초당 사이클, 1이면 1초에 한번 왕복)
    UPROPERTY(EditAnywhere, Category = "Arc Settings")
    float SwingSpeed;

    // 총 왕복 횟수
    UPROPERTY(EditAnywhere, Category = "Arc Settings")
    float TotalSwings;

    // 뷰 전환 블렌드 시간 (초)
    UPROPERTY(EditAnywhere, Category = "Arc Settings")
    float BlendTime;

    // 경과 시간 (내부용)
    float ElapsedTime;

    // 총 재생 시간 (TotalSwings / SwingSpeed로 계산)
    float TotalDuration;

    // StartArcSwing 호출 시점의 TargetActor 정면 방향 (Yaw만 사용)
    // Tick 중에 TargetActor가 회전해도 카메라 기준축이 흔들리지 않도록 고정
    float BaseYaw = 0.f;

    // 호 이동 작동 여부
    uint8 bIsSwinging : 1;
};
