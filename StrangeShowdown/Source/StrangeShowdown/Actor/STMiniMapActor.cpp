// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/STMiniMapActor.h"
#include "Kismet/GameplayStatics.h"
#include "Components/WidgetComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Controller/STPlayerController.h"
#include "Widget/STMiniMapWidget.h"
#include "GameFramework/HUD.h"
#include "Widget/STHUD.h"
#include "Item/STPickupItem.h"

// Sets default values
ASTMiniMapActor::ASTMiniMapActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	MiniMapCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("MiniMapCapture"));
	MiniMapCapture->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ASTMiniMapActor::BeginPlay()
{
	Super::BeginPlay();

	MiniMapCapture->ShowFlags.SetLighting(false);
	MiniMapCapture->ShowFlags.SetShadowFrustums(false);
	MiniMapCapture->ShowFlags.SetDynamicShadows(false);
	MiniMapCapture->ShowFlags.SetTranslucency(false);
	MiniMapCapture->ShowFlags.SetPostProcessing(false);

	// 위젯 컴포넌트 숨기기
	HiddenWidgetComponent();

	// 아이템 수집 타이머 설정(딜레이)
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		this,
		&ASTMiniMapActor::CollectItems,
		0.5f,
		false);

	// HUD 연결 타이머 설정(딜레이)
	FTimerHandle TimerHandle2;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle2,
		this,
		&ASTMiniMapActor::BringHUD,
		0.5f,
		false);
}

// Called every frame
void ASTMiniMapActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateMiniMapRotation(DeltaTime);

	UpdateItemOnMiniMap(DeltaTime);
}

void ASTMiniMapActor::CollectItems()
{
	MiniMapItems.Empty();

	TArray<AActor*> ItemActors;
	UGameplayStatics::GetAllActorsOfClass(
		GetWorld(),
		ASTPickupItem::StaticClass(),
		ItemActors);

	for (AActor* Actor : ItemActors)
	{
		MiniMapItems.Add(Cast<ASTPickupItem>(Actor));
	}
}

void ASTMiniMapActor::BringHUD()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		ASTPlayerController* STPC = Cast<ASTPlayerController>(PC);
		if (STPC)
		{
			MiniMapWidget = STPC->HUDWidget->GetMiniMapWidget();
			if (!MiniMapWidget)
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to get MiniMapWidget from HUD"));
				return;
			}
		}
	}
}

FVector2D ASTMiniMapActor::WorldToMiniMap(const FVector& WorldLocation) const
{
	FVector2D Delta(
		WorldLocation.X - GetActorLocation().X,
		WorldLocation.Y - GetActorLocation().Y);

	float MapYaw = GetActorRotation().Yaw;
	Delta = Delta.GetRotated(-MapYaw);

	float HalfWidth = MiniMapCapture->OrthoWidth * 0.5f;
	float WidgetSize = 512.f;

	float Scale = WidgetSize / MiniMapCapture->OrthoWidth;

	return Delta * Scale;
}

void ASTMiniMapActor::UpdateItemOnMiniMap(float DeltaTime)
{
	if (!MiniMapWidget) return;
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;
	APawn* PlayerPawn = PC->GetPawn();
	if (!PlayerPawn) return;

	FVector PlayerLocation = PlayerPawn->GetActorLocation();
	float ControlYaw = PC->GetControlRotation().Yaw;  // 카메라 회전값
	float HalfWidth = MiniMapCapture->OrthoWidth * 0.5f;
	const float MapSize = 300.f;
	const float HalfIcon = 8.f;

	for (ASTPickupItem* Item : MiniMapItems)
	{
		if (!Item) continue;

		FVector ItemLocation = Item->GetActorLocation();

		FVector2D Relative(
			ItemLocation.X - PlayerLocation.X,
			ItemLocation.Y - PlayerLocation.Y);

		// 카메라 ControlYaw 역회전 적용
		float Rad = FMath::DegreesToRadians(-ControlYaw);
		float Cos = FMath::Cos(Rad);
		float Sin = FMath::Sin(Rad);

		FVector2D Rotated(
			Relative.X * Cos - Relative.Y * Sin,
			Relative.X * Sin + Relative.Y * Cos);

		bool bInside =
			FMath::Abs(Rotated.X) < HalfWidth &&
			FMath::Abs(Rotated.Y) < HalfWidth;

		if (bInside)
		{
			FVector2D MiniMapPos;
			MiniMapPos.X = (Rotated.Y / HalfWidth) * (MapSize * 0.5f) + (MapSize * 0.5f);
			MiniMapPos.Y = (-Rotated.X / HalfWidth) * (MapSize * 0.5f) + (MapSize * 0.5f);

			MiniMapPos.X = FMath::Clamp(MiniMapPos.X, HalfIcon, MapSize - HalfIcon);
			MiniMapPos.Y = FMath::Clamp(MiniMapPos.Y, HalfIcon, MapSize - HalfIcon);

			MiniMapWidget->UpdateItemIcon(Item, MiniMapPos);
		}
		else
		{
			MiniMapWidget->HideItemIcon(Item);
		}
	}
}

void ASTMiniMapActor::UpdateMiniMapRotation(float DeltaTime)
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;

	APawn* Pawn = PC->GetPawn();
	if (!Pawn) return;

	// 위치 따라가기
	FVector NewLocation = Pawn->GetActorLocation() + FVector(0, 0, zPosition);
	SetActorLocation(NewLocation);

	// 플레이어 마우스 회전 가져오기
	float ControlYaw = PC->GetControlRotation().Yaw;

	// 미니맵은 반대로 회전
	FRotator TargetRot(-90.f, ControlYaw, 0.f);

	// RInterpTo
	FRotator SmoothRot = FMath::RInterpTo(
		GetActorRotation(),
		TargetRot,
		DeltaTime,
		8.f);

	SetActorRotation(SmoothRot);
}

void ASTMiniMapActor::HiddenWidgetComponent()
{
	TArray<AActor*> AllActors;
	UGameplayStatics::GetAllActorsOfClass(
		GetWorld(),
		AActor::StaticClass(),
		AllActors);

	for (AActor* Actor : AllActors)
	{
		TArray<UWidgetComponent*> Widgets;
		Actor->GetComponents<UWidgetComponent>(Widgets);

		for (UWidgetComponent* WidgetComp : Widgets)
		{
			MiniMapCapture->HiddenComponents.Add(WidgetComp);
		}
	}
}