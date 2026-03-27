// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/STBigMapActor.h"
#include "Kismet/GameplayStatics.h"
#include "Components/WidgetComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Controller/STBaseController.h"
#include "Widget/STMiniMapWidget.h"
#include "GameFramework/HUD.h"
#include "Widget/STHUD.h"
#include "Interface/STMiniMapTargetInterface.h"

// Sets default values
ASTBigMapActor::ASTBigMapActor()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	MiniMapCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("MiniMapCapture"));
	MiniMapCapture->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ASTBigMapActor::BeginPlay()
{
	Super::BeginPlay();

	SetActorLocation(FVector(0, -3500.f, zPosition));

	InitWidgetComponent();
	ApplyMiniMapHidden();

	// HUD 연결 타이머 설정(딜레이)
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		this,
		&ASTBigMapActor::BringHUD,
		0.5f,
		false);
}

// Called every frame
void ASTBigMapActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateTargetOnMiniMap(DeltaTime);
	UpdatePlayerOnMiniMap(DeltaTime);
}

void ASTBigMapActor::BringHUD()
{
	ASTBaseController* STPC = Cast<ASTBaseController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (STPC)
	{
		HUDWidget = STPC->GetHUDWidget();
		MiniMapWidget = HUDWidget->GetBigMapWidget();
		if (!MiniMapWidget)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to get BigMapWidget from HUD"));
			return;
		}
	}

	MiniMapWidget->SetIsRotationAble(false);
}

void ASTBigMapActor::RegisterBigMapTarget(AActor* Actor)
{
	if (!Actor) return;

	if (!Actor->Implements<USTMiniMapTargetInterface>())
	{
		return;
	}

	MiniMapTargets.AddUnique(Actor);

	Actor->OnDestroyed.RemoveDynamic(this, &ASTBigMapActor::OnIconDestroyed);
	Actor->OnDestroyed.AddDynamic(this, &ASTBigMapActor::OnIconDestroyed);
}

void ASTBigMapActor::OnIconDestroyed(AActor* DestroyedActor)
{
	if (!DestroyedActor) return;

	MiniMapTargets.Remove(DestroyedActor);

	if (MiniMapWidget)
	{
		MiniMapWidget->HideTargetIcon(DestroyedActor);
	}
}

FVector2D ASTBigMapActor::WorldToMiniMap(const FVector& WorldLocation) const
{
	FVector ActorLocation = GetActorLocation();

	FVector2D Relative(
		WorldLocation.Y - ActorLocation.Y,
		WorldLocation.X - ActorLocation.X);

	Relative.Y *= -1.f;

	const float WidgetSize = HUDWidget->BigMapWidget->GetDesiredSize().Y;
	const float OrthoWidth = MiniMapCapture->OrthoWidth;

	const float Scale = WidgetSize / OrthoWidth;
	Relative *= Scale;

	const float HalfSize = WidgetSize * 0.5f;
	Relative += FVector2D(HalfSize, HalfSize);

	return Relative;
}

void ASTBigMapActor::UpdateTargetOnMiniMap(float DeltaTime)
{
	if (!MiniMapWidget) return;
	if (!HUDWidget || !HUDWidget->BigMapWidget) return;

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
			WorldToMiniMap(TargetLocation);

		FVector2D Offset(-55.f, -20.f);
		MiniMapPos += Offset;

		const float WidgetSize = HUDWidget->BigMapWidget->GetDesiredSize().Y;

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

void ASTBigMapActor::UpdatePlayerOnMiniMap(float DeltaTime)
{
	if (!MiniMapWidget) return;

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC) return;

	APawn* PlayerPawn = PC->GetPawn();
	if (!PlayerPawn) return;

	FVector2D MiniMapPos =
		WorldToMiniMap(PlayerPawn->GetActorLocation());

	// 오프셋(-400, -400)
	FVector2D Offset(-400.f, -400.f);
	MiniMapPos += Offset;

	MiniMapWidget->UpdatePlayerIcon(MiniMapPos);
}

void ASTBigMapActor::InitWidgetComponent()
{
	if (!MiniMapCapture) return;

	MiniMapCapture->ProjectionType = ECameraProjectionMode::Orthographic;
	MiniMapCapture->OrthoWidth = 12000.f;

	MiniMapCapture->bCaptureEveryFrame = true;
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

void ASTBigMapActor::ApplyMiniMapHidden()
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