// Fill out your copyright notice in the Description page of Project Settings.


#include "VictoryCamera.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/PlayerController.h"

AVictoryCamera::AVictoryCamera()
{
    PrimaryActorTick.bCanEverTick = true;

    // 카메라 컴포넌트 생성
    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
    RootComponent = CameraComponent;

    // 초기값 설정
	TargetActor = nullptr;
    Distance = 400.f;
    BaseHeight = 150.f;
    SwingAngle = 60.f;
    HeightVariance = 80.f;
    SwingSpeed = 0.5f;
    TotalSwings = 4.f;
    BlendTime = 1.5f;
    ElapsedTime = 0.f;
    TotalDuration = 0.f;
	bIsSwinging = false;
}

void AVictoryCamera::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bIsSwinging || !TargetActor)
    {
        return;
    }

    ElapsedTime += DeltaTime;

    // 총 재생 시간이 지나면 정지
    if (ElapsedTime >= TotalDuration)
    {
        bIsSwinging = false;
        UE_LOG(LogTemp, Log, TEXT("VictoryCamera: Arc swing complete."));
        return;
    }

    // 사인파로 -1 ~ +1 사이를 부드럽게 왕복
    const float SineValue = FMath::Sin(ElapsedTime * SwingSpeed * 2.f * PI);

    // TargetActor 정면 방향(BaseYaw) 기준으로 좌우 호 각도 계산
    const float FinalYawDeg = BaseYaw + (SineValue * SwingAngle);
    const float FinalYawRad = FMath::DegreesToRadians(FinalYawDeg);

    // 타겟 위치 기준으로 카메라 위치 계산
    const FVector TargetLocation = TargetActor->GetActorLocation();

    FVector NewLocation;
    NewLocation.X = TargetLocation.X + FMath::Cos(FinalYawRad) * Distance;
    NewLocation.Y = TargetLocation.Y + FMath::Sin(FinalYawRad) * Distance;
    NewLocation.Z = TargetLocation.Z + BaseHeight + (SineValue * HeightVariance);

    SetActorLocation(NewLocation);

    // 항상 타겟을 바라보도록 회전
    const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(NewLocation, TargetLocation);
    SetActorRotation(LookAtRotation);
}

void AVictoryCamera::StartArcSwing(AActor* InTargetActor)
{
    TargetActor = InTargetActor;
    ElapsedTime = 0.f;
    TotalDuration = TotalSwings / SwingSpeed;

    // 시작 시점의 TargetActor 정면 Yaw를 기준축으로 고정
    BaseYaw = TargetActor->GetActorRotation().Yaw;

    // 첫 Swing 위치 (SineValue=0 이므로 타겟 정면, 기본 높이)
    const float InitialYawRad = FMath::DegreesToRadians(BaseYaw);
    const FVector TargetLocation = TargetActor->GetActorLocation();

    FVector StartLocation;
    StartLocation.X = TargetLocation.X + FMath::Cos(InitialYawRad) * Distance;
    StartLocation.Y = TargetLocation.Y + FMath::Sin(InitialYawRad) * Distance;
    StartLocation.Z = TargetLocation.Z + BaseHeight;

    // 카메라를 첫 Swing 위치로 즉시 이동
    SetActorLocation(StartLocation);

    // 처음부터 타겟을 바라보도록 회전
    const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(StartLocation, TargetLocation);
    SetActorRotation(LookAtRotation);

    // BlendTime 0 → 즉시 전환, 카메라가 이미 첫 Swing 위치에 있으므로 끊김 없음
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PC)
    {
        PC->SetViewTarget(this);
    }

    // 즉시 Swing 시작
    bIsSwinging = true;

    float DelaySpeed = 0.2f;
    UGameplayStatics::SetGlobalTimeDilation(GetWorld(), DelaySpeed);

    FTimerHandle TimerHandle;
    GetWorldTimerManager().SetTimer(TimerHandle, [this]()
    {
        UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
		}, 2.0f * DelaySpeed, false);
}