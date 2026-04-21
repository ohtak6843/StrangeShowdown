#include "Component/STTargetIndicatorComponent.h"
#include "Component/STAttackTraceComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
USTTargetIndicatorComponent::USTTargetIndicatorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USTTargetIndicatorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!AttackTraceWidgetComponent || !AttackTraceWidgetComponent->IsVisible())
		return;

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC) return;

	FVector CameraLocation;
	FRotator CameraRotation;
	PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

	FVector ToCamera = CameraLocation - AttackTraceWidgetComponent->GetComponentLocation();
	FRotator LookAtRotation = FRotationMatrix::MakeFromX(ToCamera).Rotator();
	AttackTraceWidgetComponent->SetWorldRotation(LookAtRotation);

	USkeletalMeshComponent* MeshComp = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();
	if (!MeshComp) return;

	FVector HeadLocation = MeshComp->GetSocketLocation(TEXT("head"));

	FVector Forward = CameraRotation.Vector();
	FVector NewLocation = HeadLocation + Forward * -20.f;

	AttackTraceWidgetComponent->SetWorldLocation(NewLocation);
}

// Called when the game starts
void USTTargetIndicatorComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	if (!Owner) return;

	USkeletalMeshComponent* MeshComp = Owner->FindComponentByClass<USkeletalMeshComponent>();
	if (!MeshComp) return;

	AttackTraceWidgetComponent = NewObject<UWidgetComponent>(Owner, TEXT("AttackTraceWidgetComponent"));

	AttackTraceWidgetComponent->SetupAttachment(MeshComp, TEXT("head"));

	AttackTraceWidgetComponent->RegisterComponent();
	AttackTraceWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AttackTraceWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	AttackTraceWidgetComponent->SetDrawSize(FVector2D(50.f, 50.f));
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
	}
	else
	{
		AttackTraceWidgetComponent->SetVisibility(false);
	}
}

void USTTargetIndicatorComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	Attackers.Empty();
}