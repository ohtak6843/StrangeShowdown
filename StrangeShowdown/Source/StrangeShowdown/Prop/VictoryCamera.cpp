// File: Source/StrangeShowdown/Private/VictoryCamera.cpp

#include "VictoryCamera.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/PlayerController.h"

AVictoryCamera::AVictoryCamera()
{
    PrimaryActorTick.bCanEverTick = true;

    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
    RootComponent = CameraComponent;

    TargetActor = nullptr;
    Distance = 400.f;
    BaseHeight = 150.f;
    BlendTime = 1.5f;
    ElapsedTime = 0.f;
    TotalDuration = 0.f;
    PhaseElapsed = 0.f;
    CurrentPhase = 0;
    BaseYaw = 0.f;
    bIsSwinging = false;

    HoldDuration = 2.0f;
    TransitDuration = 1.2f;

    ArcT_LeftUp = 0.0f;
    ArcT_RightDown = 0.5f;
    ArcT_Front = 0.75f;

    DriftAmount = 0.08f;
}

void AVictoryCamera::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bIsSwinging || !IsValid(TargetActor))
    {
        return;
    }

    PhaseElapsed += DeltaTime;

    float TargetT = ArcT_LeftUp;

    switch (CurrentPhase)
    {
    case 0: // LeftUp 체류 + 드리프트
    {
        // 사인파로 ArcT_LeftUp 근처를 왕복
        const float Drift = FMath::Sin(PhaseElapsed * PI / HoldDuration) * DriftAmount;
        TargetT = ArcT_LeftUp + Drift;

        if (PhaseElapsed >= HoldDuration)
        {
            CurrentPhase = 2;
            PhaseElapsed = 0.f;
            TargetT = ArcT_RightDown;
        }
        break;
    }

    case 2: // RightDown 체류 + 드리프트
    {
        const float Drift = FMath::Sin(PhaseElapsed * PI / HoldDuration) * DriftAmount;
        TargetT = ArcT_RightDown + Drift;

        if (PhaseElapsed >= HoldDuration)
        {
            CurrentPhase = 4;
            PhaseElapsed = 0.f;
            TargetT = ArcT_Front;
        }
        break;
    }

    case 4: // Front 체류/종료
        TargetT = ArcT_Front;
        bIsSwinging = false;
        UE_LOG(LogTemp, Log, TEXT("VictoryCamera: Sequence complete."));
        return;

    default:
        return;
    }

    const FVector NewLocation = CalcArcPosition(TargetT);
    SetActorLocation(NewLocation);

    const FVector TargetLocation = TargetActor->GetActorLocation();
    const FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(NewLocation, TargetLocation);
    SetActorRotation(LookAt);
}

void AVictoryCamera::StartArcSwing(AActor* InTargetActor)
{
    if (!IsValid(InTargetActor))
    {
        UE_LOG(LogTemp, Warning, TEXT("VictoryCamera: StartArcSwing - InTargetActor is null."));
        return;
    }

    TargetActor = InTargetActor;
    ElapsedTime = 0.f;
    PhaseElapsed = 0.f;
    CurrentPhase = 0;
    BaseYaw = TargetActor->GetActorRotation().Yaw;

    const FVector StartLocation = CalcArcPosition(ArcT_LeftUp);
    SetActorLocation(StartLocation);

    const FVector TargetLocation = TargetActor->GetActorLocation();
    const FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(StartLocation, TargetLocation);
    SetActorRotation(LookAt);

    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (IsValid(PC))
    {
        PC->SetViewTarget(this);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("VictoryCamera: PlayerController not found."));
    }

    bIsSwinging = true;
}

FVector AVictoryCamera::CalcArcPosition(float T) const
{
    if (!IsValid(TargetActor))
    {
        return FVector::ZeroVector;
    }

    // 타겟 정면 반대 방향 기준 (카메라가 정면을 바라보도록)
    const float Yaw = FMath::DegreesToRadians(BaseYaw + 180.f);
    const FVector Forward(FMath::Cos(Yaw), FMath::Sin(Yaw), 0.f);
    const FVector Right(-FMath::Sin(Yaw), FMath::Cos(Yaw), 0.f);

    // 25도 기울어진 대원 기저벡터
    const float TiltRad = FMath::DegreesToRadians(25.f);
    const FVector U = (-Right * FMath::Cos(TiltRad) + FVector(0.f, 0.f, 1.f) * FMath::Sin(TiltRad));
    const FVector V = Forward;

    const float Angle = T * 2.f * PI;
    const FVector Dir = U * FMath::Cos(Angle) + V * FMath::Sin(Angle);

    const FVector TargetLocation = TargetActor->GetActorLocation()
        + FVector(0.f, 0.f, BaseHeight);

    return TargetLocation + Dir * Distance;
}