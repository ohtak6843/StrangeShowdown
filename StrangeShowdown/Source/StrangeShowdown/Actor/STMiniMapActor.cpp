#include "Actor/STMiniMapActor.h"
#include "Kismet/GameplayStatics.h"
#include "Components/WidgetComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Controller/STPlayerController.h"
#include "Widget/STMiniMapWidget.h"
#include "UI/STHUDWidget.h"
#include "Interface/STMiniMapTargetInterface.h"
#include "Interface/STControllerHUDInterface.h"

// Sets default values
ASTMiniMapActor::ASTMiniMapActor()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	MiniMapCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("MiniMapCapture"));
	MiniMapCapture->SetupAttachment(RootComponent);
}

void ASTMiniMapActor::BeginPlay()
{
	Super::BeginPlay();

	InitWidgetComponent();
	ApplyMiniMapHidden();

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		this,
		&ASTMiniMapActor::BringHUD,
		0.5f,
		false);
}

void ASTMiniMapActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateMiniMapRotation(DeltaTime);
	UpdateTargetOnMiniMap(DeltaTime);
}

void ASTMiniMapActor::BringHUD()
{
	ISTControllerHUDInterface* PC = Cast<ISTControllerHUDInterface>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PC)
	{
		MiniMapWidget = PC->GetMiniMapWidget();
		if (!MiniMapWidget)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to get MiniMapWidget from HUD"));
		}
	}
}

void ASTMiniMapActor::RegisterMiniMapTarget(AActor* Actor)
{
	if (!Actor) return;

	if (!Actor->Implements<USTMiniMapTargetInterface>())
	{
		return;
	}

	MiniMapTargets.AddUnique(Actor);

	Actor->OnDestroyed.RemoveDynamic(this, &ASTMiniMapActor::OnIconDestroyed);
	Actor->OnDestroyed.AddDynamic(this, &ASTMiniMapActor::OnIconDestroyed);
}

void ASTMiniMapActor::OnIconDestroyed(AActor* DestroyedActor)
{
	if (!DestroyedActor) return;

	MiniMapTargets.Remove(DestroyedActor);

	if (MiniMapWidget)
	{
		MiniMapWidget->HideTargetIcon(DestroyedActor);
	}
}

FVector2D ASTMiniMapActor::WorldToMiniMap(const FVector& ItemLocation, const FVector& PlayerLocation, float PlayerYaw) const
{
	FVector2D Relative(
		ItemLocation.Y - PlayerLocation.Y,
		ItemLocation.X - PlayerLocation.X);

	Relative.Y *= -1.f;

	float CurrentYaw = GetActorRotation().Yaw;
	Relative = Relative.GetRotated(-CurrentYaw);

	if (nullptr == MiniMapWidget || !MiniMapCapture)
	{
		return FVector2D::ZeroVector;
	}

	const float WidgetSize = MiniMapWidget->GetDesiredSize().Y;
	const float OrthoWidth = MiniMapCapture->OrthoWidth;

	const float Scale = WidgetSize / OrthoWidth;
	Relative *= Scale;

	const float HalfSize = WidgetSize * 0.5f;
	Relative += FVector2D(HalfSize, HalfSize);

	return Relative;
}

void ASTMiniMapActor::UpdateMiniMapRotation(float DeltaTime)
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC) return;

	APawn* Pawn = PC->GetPawn();
	if (!Pawn) return;

	FVector NewLocation = Pawn->GetActorLocation() + FVector(0, 0, zPosition);
	SetActorLocation(NewLocation);

	float ControlYaw = PC->GetControlRotation().Yaw;

	FRotator TargetRot(-90.f, ControlYaw, 0.f);

	FRotator SmoothRot = FMath::RInterpTo(
		GetActorRotation(),
		TargetRot,
		DeltaTime,
		8.f);

	SetActorRotation(SmoothRot);
}

void ASTMiniMapActor::UpdateTargetOnMiniMap(float DeltaTime)
{
	if (!MiniMapWidget) return;

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC) return;

	APawn* PlayerPawn = PC->GetPawn();
	if (!PlayerPawn) return;

	float CurrentYaw = GetActorRotation().Yaw;

	for (int32 i = MiniMapTargets.Num() - 1; i >= 0; --i)
	{
		AActor* TargetActor = MiniMapTargets[i].Get();
		if (!TargetActor)
		{
			MiniMapTargets.RemoveAt(i);
			continue;
		}

		if (!TargetActor->Implements<USTMiniMapTargetInterface>())
		{
			continue;
		}

		const FVector TargetLocation =
			ISTMiniMapTargetInterface::Execute_GetMiniMapLocation(TargetActor);

		FVector2D MiniMapPos =
			WorldToMiniMap(
				TargetLocation,
				PlayerPawn->GetActorLocation(),
				CurrentYaw);

		FVector2D Offset(-55.f, -20.f);
		MiniMapPos += Offset;

		const float WidgetSize = MiniMapWidget->GetDesiredSize().Y;

		const float MinX = Offset.X + 10.f;
		const float MaxX = Offset.X + WidgetSize - 15.f;
		const float MinY = Offset.Y + 10.f;
		const float MaxY = Offset.Y + WidgetSize - 10.f;

		if (MiniMapPos.X < MinX || MiniMapPos.X > MaxX || MiniMapPos.Y < MinY || MiniMapPos.Y > MaxY)
		{
			MiniMapWidget->HideTargetIcon(TargetActor);
		}
		else
		{
			MiniMapWidget->UpdateTargetIcon(TargetActor, MiniMapPos);
		}
	}
}

void ASTMiniMapActor::InitWidgetComponent()
{
	if (!MiniMapCapture) return;

	MiniMapCapture->ProjectionType = ECameraProjectionMode::Orthographic;
	MiniMapCapture->OrthoWidth = 6000.f;

	MiniMapCapture->bCaptureEveryFrame = false;
	MiniMapCapture->bCaptureOnMovement = true;
	MiniMapCapture->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;

	MiniMapCapture->LODDistanceFactor = 3.0f;
	MiniMapCapture->MaxViewDistanceOverride = 5000.f;
	MiniMapCapture->PostProcessBlendWeight = 0.0f;

	auto& Flags = MiniMapCapture->ShowFlags;

	Flags.SetLighting(false);
	Flags.SetDynamicShadows(false);
	Flags.SetShadowFrustums(false);
	Flags.SetPostProcessing(false);
	Flags.SetBloom(false);
	Flags.SetFog(false);
	Flags.SetVolumetricFog(false);
	Flags.SetAtmosphere(false);
	Flags.SetSkyLighting(false);
	Flags.SetAntiAliasing(false);

	Flags.SetSkeletalMeshes(false);
	Flags.SetParticles(false);

	Flags.SetStaticMeshes(true);
	Flags.SetLandscape(true);

	MiniMapCapture->CaptureScene();
}

void ASTMiniMapActor::ApplyMiniMapHidden()
{
	TArray<AActor*> AllActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), AllActors);

	for (AActor* Actor : AllActors)
	{
		if (!Actor) continue;

		if (Actor->ActorHasTag(TEXT("MiniMapHidden")))
		{
			MiniMapCapture->HiddenActors.Add(Actor);
		}

		TArray<UWidgetComponent*> Widgets;
		Actor->GetComponents<UWidgetComponent>(Widgets);

		for (UWidgetComponent* WidgetComp : Widgets)
		{
			if (WidgetComp)
			{
				MiniMapCapture->HiddenComponents.Add(WidgetComp);
			}
		}
	}
}