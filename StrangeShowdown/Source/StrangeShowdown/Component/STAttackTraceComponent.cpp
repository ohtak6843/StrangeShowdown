#include "Component/STAttackTraceComponent.h"
#include "Component/STTargetIndicatorComponent.h"
#include "Character/Player/STFieldPlayer.h"
#include "Character/Player/STLocalPlayer.h"
#include "Character/Sheriff/STLocalSheriff.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
USTAttackTraceComponent::USTAttackTraceComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USTAttackTraceComponent::BeginPlay()
{
	Super::BeginPlay();
}

void USTAttackTraceComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AActor* Owner = GetOwner();
	if (!Owner)
		return;

	// 조준 상태 체크
	if (ASTLocalPlayer* LocalPlayer = Cast<ASTLocalPlayer>(Owner))
	{
		if (!LocalPlayer->HasAnyState(EPlayerState::Aiming))
		{
			SetTracingTarget(nullptr);
			return;
		}
	}
	else if (ASTLocalSheriff* LocalSheriff = Cast<ASTLocalSheriff>(Owner))
	{
		if (!LocalSheriff->HasAnyState(ESheriffState::Aiming))
		{
			SetTracingTarget(nullptr);
			return;
		}
	}
	else
	{
		SetTracingTarget(nullptr);
		return;
	}

	ASTFieldPlayer* NewTarget = FindTargetInSight();
	SetTracingTarget(NewTarget);
}

void USTAttackTraceComponent::SetTracingTarget(ASTFieldPlayer* NewTarget)
{
	if (TracingFieldPlayer == NewTarget)
		return;

	// 이전 타겟 제거
	if (TracingFieldPlayer)
	{
		if (USTTargetIndicatorComponent* Indicator =
			TracingFieldPlayer->FindComponentByClass<USTTargetIndicatorComponent>())
		{
			Indicator->RemoveAttacker(this);
		}
	}

	TracingFieldPlayer = NewTarget;

	// 새로운 타겟 등록
	if (TracingFieldPlayer)
	{
		if (USTTargetIndicatorComponent* Indicator =
			TracingFieldPlayer->FindComponentByClass<USTTargetIndicatorComponent>())
		{
			Indicator->AddAttacker(this);
		}
	}
}

ASTFieldPlayer* USTAttackTraceComponent::FindTargetInSight() const
{
	UWorld* World = GetWorld();
	if (!World)
		return nullptr;

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC)
		return nullptr;

	FVector CameraLocation;
	FRotator CameraRotation;
	PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

	const FVector Start = CameraLocation;
	const FVector End = Start + CameraRotation.Vector() * TraceDistance;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	TArray<FHitResult> HitResults;
	bool bHit = World->SweepMultiByChannel(
		HitResults,
		Start,
		End,
		FQuat::Identity,
		ECC_GameTraceChannel1,
		FCollisionShape::MakeSphere(60.f),
		Params
	);

	if (!bHit)
		return nullptr;

	int32 ViewX, ViewY;
	PC->GetViewportSize(ViewX, ViewY);
	const FVector2D ScreenCenter(ViewX * 0.5f, ViewY * 0.5f);

	ASTFieldPlayer* BestTarget = nullptr;
	float BestScore = FLT_MAX;

	for (const FHitResult& Hit : HitResults)
	{
		ASTFieldPlayer* Player = Cast<ASTFieldPlayer>(Hit.GetActor());
		if (!Player)
			continue;

		FVector2D ScreenPos;
		if (!PC->ProjectWorldLocationToScreen(Player->GetActorLocation(), ScreenPos))
			continue;

		const float DistToCenter = FVector2D::Distance(ScreenPos, ScreenCenter);
		if (DistToCenter < BestScore)
		{
			BestScore = DistToCenter;
			BestTarget = Player;
		}
	}

	return BestTarget;
}

void USTAttackTraceComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (TracingFieldPlayer)
	{
		if (USTTargetIndicatorComponent* Indicator =
			TracingFieldPlayer->FindComponentByClass<USTTargetIndicatorComponent>())
		{
			Indicator->RemoveAttacker(this);
		}
	}
}