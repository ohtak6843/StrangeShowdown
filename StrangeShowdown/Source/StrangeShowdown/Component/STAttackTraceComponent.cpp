#include "Component/STAttackTraceComponent.h"
#include "Character/Player/STFieldPlayer.h"
#include "Character/Player/STLocalPlayer.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "Components/SkeletalMeshComponent.h"

USTAttackTraceComponent::USTAttackTraceComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USTAttackTraceComponent::BeginPlay()
{
	Super::BeginPlay();

	OnTargetAcquired.AddDynamic(this, &USTAttackTraceComponent::HandleTargetAcquired);
	OnTargetLost.AddDynamic(this, &USTAttackTraceComponent::HandleTargetLost);

	AttackTraceWidgetComponent = NewObject<UWidgetComponent>(GetOwner());
	AttackTraceWidgetComponent->RegisterComponent();
	AttackTraceWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AttackTraceWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	AttackTraceWidgetComponent->SetDrawSize(FVector2D(100.f, 100.f));
	// 앞뒤면 모두 렌더링
	AttackTraceWidgetComponent->SetTwoSided(true);
	AttackTraceWidgetComponent->SetVisibility(false);
}

void USTAttackTraceComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 줌 상태일때만 트레이스
	if(Cast<ASTLocalPlayer>(GetOwner()))
	{
		ASTLocalPlayer* LocalPlayer = Cast<ASTLocalPlayer>(GetOwner());
		if (!(LocalPlayer->HasAnyState(EPlayerStateFlag::Aiming)))
		{
			SetTracingTarget(nullptr);
			return;
		}
	}
	ASTFieldPlayer* NewTarget = FindTargetInSight();
	SetTracingTarget(NewTarget);
}

void USTAttackTraceComponent::HandleTargetAcquired(ASTFieldPlayer* NewTarget)
{
	if (!AttackTraceWidgetComponent || !AttackTraceWidgetClass)
		return;

	AttackTraceWidgetComponent->SetWidgetClass(AttackTraceWidgetClass);

	USkeletalMeshComponent* Mesh = NewTarget->GetMesh();
	if (!Mesh)
		return;

	AttackTraceWidgetComponent->AttachToComponent(
		Mesh,
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		TEXT("spine_03")
	);

	AttackTraceWidgetComponent->SetRelativeLocation(WidgetOffset);
	AttackTraceWidgetComponent->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
	AttackTraceWidgetComponent->SetVisibility(true);
}

void USTAttackTraceComponent::HandleTargetLost()
{
	if (!AttackTraceWidgetComponent)
		return;

	AttackTraceWidgetComponent->SetVisibility(false);
	AttackTraceWidgetComponent->DetachFromComponent(
		FDetachmentTransformRules::KeepWorldTransform
	);
}

void USTAttackTraceComponent::SetTracingTarget(ASTFieldPlayer* NewTarget)
{
	if (TracingFieldPlayer == NewTarget)
		return;

	// 새로운 타겟으로 변경
	TracingFieldPlayer = NewTarget;

	// 델리게이트 브로드캐스트
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

	// 설정된 채널로 스윕 트레이스 수행
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	// 다중 히트 결과 수집
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

	// 화면 중심 좌표 계산
	int32 ViewX, ViewY;
	PC->GetViewportSize(ViewX, ViewY);
	const FVector2D ScreenCenter(ViewX * 0.5f, ViewY * 0.5f);

	// 가장 적합한 타겟
	ASTFieldPlayer* BestTarget = nullptr;
	float BestScore = FLT_MAX;

	// 모든 히트 결과 순회해서 타겟 찾기
	for (const FHitResult& Hit : HitResults)
	{
		ASTFieldPlayer* Player = Cast<ASTFieldPlayer>(Hit.GetActor());
		if (!Player)
			continue;

		// 월드 위치를 화면 좌표로 변환
		FVector2D ScreenPos;
		if (!PC->ProjectWorldLocationToScreen(Player->GetActorLocation(), ScreenPos))
			continue;

		// 화면 중심과의 거리 계산
		const float DistToCenter = FVector2D::Distance(ScreenPos, ScreenCenter);
		if (DistToCenter < BestScore)
		{
			BestScore = DistToCenter;
			BestTarget = Player;
		}
	}

	return BestTarget;
}