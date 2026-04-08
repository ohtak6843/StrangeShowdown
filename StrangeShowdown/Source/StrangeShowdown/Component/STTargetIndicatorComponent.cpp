#include "Component/STTargetIndicatorComponent.h"
#include "Component/STAttackTraceComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
USTTargetIndicatorComponent::USTTargetIndicatorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USTTargetIndicatorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{

}

// Called when the game starts
void USTTargetIndicatorComponent::BeginPlay()
{
	Super::BeginPlay();

	AttackTraceWidgetComponent = NewObject<UWidgetComponent>(GetOwner(), TEXT("AttackTraceWidgetComponent"));

	AttackTraceWidgetComponent->SetupAttachment(GetOwner()->GetRootComponent());

	AttackTraceWidgetComponent->RegisterComponent();
	AttackTraceWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AttackTraceWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	AttackTraceWidgetComponent->SetDrawSize(FVector2D(100.f, 100.f));
	AttackTraceWidgetComponent->SetTwoSided(true);
	AttackTraceWidgetComponent->SetVisibility(false);

	AttackTraceWidgetComponent->SetRelativeLocation(WidgetOffset);
}

void USTTargetIndicatorComponent::AddAttacker(USTAttackTraceComponent* Attacker)
{
	if (!Attacker)
		return;

	Attackers.Add(Attacker);
	UpdateUI();
}

void USTTargetIndicatorComponent::RemoveAttacker(USTAttackTraceComponent* Attacker)
{
	if (!Attacker)
		return;

	Attackers.Remove(Attacker);
	UpdateUI();
}

void USTTargetIndicatorComponent::UpdateUI()
{
	if (!AttackTraceWidgetComponent)
		return;

	if (AttackTraceWidgetClass)
	{
		AttackTraceWidgetComponent->SetWidgetClass(AttackTraceWidgetClass);
	}

	UE_LOG(LogTemp, Log, TEXT("Updating Target Indicator UI. Number of Attackers: %d"), Attackers.Num());

	if (Attackers.Num() > 0)
	{
		AttackTraceWidgetComponent->SetVisibility(true);
		// AttackTraceWidget이 널인지 확인

		if (AttackTraceWidgetComponent->GetUserWidgetObject())
		{
			UE_LOG(LogTemp, Log, TEXT("Attack Trace Widget is valid."));
			// 위젯에 공격자 수 업데이트하는 로직 추가 가능
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Attack Trace Widget is null."));
		}
	}
	else
	{
		AttackTraceWidgetComponent->SetVisibility(false);
		UE_LOG(LogTemp, Log, TEXT("Target Indicator Hidden"));
	}
}

void USTTargetIndicatorComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	Attackers.Empty();
}