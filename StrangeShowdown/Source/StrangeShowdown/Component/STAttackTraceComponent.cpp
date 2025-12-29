#include "Component/STAttackTraceComponent.h"
#include "Player/STFieldPlayer.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"

USTAttackTraceComponent::USTAttackTraceComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USTAttackTraceComponent::BeginPlay()
{
	Super::BeginPlay();

	// 상태 변화 델리게이트 바인딩
	OnTargetAcquired.AddDynamic(
		this,
		&USTAttackTraceComponent::HandleTargetAcquired
	);

	OnTargetLost.AddDynamic(
		this,
		&USTAttackTraceComponent::HandleTargetLost
	);
}

void USTAttackTraceComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 1. 타겟 판단
	ASTFieldPlayer* NewTarget = FindTargetInSight();

	// 2. 상태 변화 처리 (델리게이트 발생 지점)
	SetTracingTarget(NewTarget);

	// 3. UI 위치 갱신 (타겟이 있을 때만)
	if (TracingFieldPlayer && AttackTraceWidget)
	{
		// FieldPlayer의 메쉬를 가져와서 머리 위치를 계산하면 좋을 거같음
		const FVector WorldPos =
			TracingFieldPlayer->GetActorLocation() + FVector(0.f, 0.f, 0.f);

		if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
		{
			FVector2D ScreenPos;
			if (PC->ProjectWorldLocationToScreen(WorldPos, ScreenPos))
			{
				AttackTraceWidget->SetPositionInViewport(ScreenPos);
			}
		}
	}
}

void USTAttackTraceComponent::HandleTargetAcquired(ASTFieldPlayer* NewTarget)
{
	if (AttackTraceWidget)
		return;

	if (!AttackTraceWidgetClass)
		return;

	// 위젯 생성
	AttackTraceWidget = CreateWidget<UUserWidget>(
		GetWorld(),
		AttackTraceWidgetClass
	);

	if (AttackTraceWidget)
	{
		AttackTraceWidget->AddToViewport();
	}
}

void USTAttackTraceComponent::HandleTargetLost()
{
	if (!AttackTraceWidget)
		return;

	// 위젯 삭제
	AttackTraceWidget->RemoveFromParent();
	AttackTraceWidget = nullptr;
}

void USTAttackTraceComponent::SetTracingTarget(ASTFieldPlayer* NewTarget)
{
	if (TracingFieldPlayer == NewTarget)
		return;

	// 타겟 변경
	TracingFieldPlayer = NewTarget;

	// 델리게이트를 받아서 처리
	if (TracingFieldPlayer)
	{
		OnTargetAcquired.Broadcast(TracingFieldPlayer);
	}
	else
	{
		OnTargetLost.Broadcast();
	}
}

ASTFieldPlayer* USTAttackTraceComponent::FindTargetInSight() const
{
	UWorld* World = GetWorld();
	if (!World)
		return nullptr;

	APlayerController* PC = World->GetFirstPlayerController();
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