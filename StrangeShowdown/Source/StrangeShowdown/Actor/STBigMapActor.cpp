// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/STBigMapActor.h"
#include "Kismet/GameplayStatics.h"
#include "Components/WidgetComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Controller/STPlayerController.h"
#include "Widget/STMiniMapWidget.h"
#include "GameFramework/HUD.h"
#include "UI/STHUDWidget.h"
#include "Interface/STMiniMapTargetInterface.h"
#include "Interface/STControllerHUDInterface.h"

// Sets default values
ASTBigMapActor::ASTBigMapActor()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	BigMapCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("MiniMapCapture"));
	BigMapCapture->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ASTBigMapActor::BeginPlay()
{
	Super::BeginPlay();

	SetActorLocation(FVector(0, -3500.f, zPosition));

	InitWidgetComponent();
	ApplyBigMapHidden();

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

	UpdateTargetOnBigMap(DeltaTime);
	UpdatePlayerOnBigMap(DeltaTime);
}

void ASTBigMapActor::BringHUD()
{
	ISTControllerHUDInterface* PC = Cast<ISTControllerHUDInterface>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PC)
	{
		BigMapWidget = PC->GetBigMapWidget();
		if (BigMapWidget)
		{
			BigMapWidget->SetIsRotationAble(false);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to get BigMapWidget from HUD"));
			return;
		}
	}
}

void ASTBigMapActor::RegisterBigMapTarget(AActor* Actor)
{
	if (!Actor) return;

	if (!Actor->Implements<USTMiniMapTargetInterface>())
	{
		return;
	}

	BigMapTargets.AddUnique(Actor);

	Actor->OnDestroyed.RemoveDynamic(this, &ASTBigMapActor::OnIconDestroyed);
	Actor->OnDestroyed.AddDynamic(this, &ASTBigMapActor::OnIconDestroyed);
}

void ASTBigMapActor::OnIconDestroyed(AActor* DestroyedActor)
{
	if (!DestroyedActor) return;

	BigMapTargets.Remove(DestroyedActor);

	if (BigMapWidget)
	{
		BigMapWidget->HideTargetIcon(DestroyedActor);
	}
}

FVector2D ASTBigMapActor::WorldToMiniMap(const FVector& WorldLocation) const
{
	FVector ActorLocation = GetActorLocation();

	FVector2D Relative(
		WorldLocation.Y - ActorLocation.Y,
		WorldLocation.X - ActorLocation.X);

	Relative.Y *= -1.f;

	const float WidgetSize = BigMapWidget->GetDesiredSize().Y;
	const float OrthoWidth = BigMapCapture->OrthoWidth;

	const float Scale = WidgetSize / OrthoWidth;
	Relative *= Scale;

	const float HalfSize = WidgetSize * 0.5f;
	Relative += FVector2D(HalfSize, HalfSize);

	return Relative;
}

void ASTBigMapActor::UpdateTargetOnBigMap(float DeltaTime)
{
	if (!BigMapWidget) return;

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC) return;

	APawn* PlayerPawn = PC->GetPawn();
	if (!PlayerPawn) return;

	float CurrentYaw = GetActorRotation().Yaw;

	for (int32 i = BigMapTargets.Num() - 1; i >= 0; --i)
	{
		AActor* TargetActor = BigMapTargets[i].Get();
		if (!TargetActor)
		{
			BigMapTargets.RemoveAt(i);
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

		const float WidgetSize = BigMapWidget->GetDesiredSize().Y;

		const float MinX = Offset.X + 10.f;
		const float MaxX = Offset.X + WidgetSize - 15.f;
		const float MinY = Offset.Y + 10.f;
		const float MaxY = Offset.Y + WidgetSize - 10.f;

		if (MiniMapPos.X < MinX || MiniMapPos.X > MaxX || MiniMapPos.Y < MinY || MiniMapPos.Y > MaxY)
		{
			BigMapWidget->HideTargetIcon(TargetActor);
		}
		else
		{
			BigMapWidget->UpdateTargetIcon(TargetActor, MiniMapPos);
		}
	}
}

void ASTBigMapActor::UpdatePlayerOnBigMap(float DeltaTime)
{
	if (!BigMapWidget) return;

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC) return;

	APawn* PlayerPawn = PC->GetPawn();
	if (!PlayerPawn) return;

	FVector2D MiniMapPos =
		WorldToMiniMap(PlayerPawn->GetActorLocation());

	// 오프셋(-400, -400)
	FVector2D Offset(-400.f, -400.f);
	MiniMapPos += Offset;

	BigMapWidget->UpdatePlayerIcon(MiniMapPos);
}

void ASTBigMapActor::InitWidgetComponent()
{
	if (!BigMapCapture) return;

	BigMapCapture->ProjectionType = ECameraProjectionMode::Orthographic;
	BigMapCapture->OrthoWidth = 12000.f;

	BigMapCapture->bCaptureEveryFrame = true;
	BigMapCapture->bCaptureOnMovement = true;
	BigMapCapture->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;

	BigMapCapture->LODDistanceFactor = 3.0f;
	BigMapCapture->MaxViewDistanceOverride = 5000.f;
	BigMapCapture->PostProcessBlendWeight = 0.0f;

	auto& Flags = BigMapCapture->ShowFlags;

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

	BigMapCapture->CaptureScene();
}

void ASTBigMapActor::ApplyBigMapHidden()
{
	TArray<AActor*> AllActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), AllActors);

	for (AActor* Actor : AllActors)
	{
		if (!Actor) continue;

		if (Actor->ActorHasTag(TEXT("MiniMapHidden")))
		{
			BigMapCapture->HiddenActors.Add(Actor);
		}

		TArray<UWidgetComponent*> Widgets;
		Actor->GetComponents<UWidgetComponent>(Widgets);

		for (UWidgetComponent* WidgetComp : Widgets)
		{
			if (WidgetComp)
			{
				BigMapCapture->HiddenComponents.Add(WidgetComp);
			}
		}
	}
}