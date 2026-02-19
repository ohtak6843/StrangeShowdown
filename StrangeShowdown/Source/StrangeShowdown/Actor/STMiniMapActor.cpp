// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/STMiniMapActor.h"
#include "Kismet/GameplayStatics.h"
#include "Components/WidgetComponent.h"
#include "Components/SceneCaptureComponent2D.h"
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
	// 아이템 위치 업데이트
	for (ASTPickupItem* Item : MiniMapItems)
	{
		if (!Item) continue;

		FVector ItemLocation = Item->GetActorLocation();

		FVector2D Delta(
			ItemLocation.X - GetActorLocation().X,
			ItemLocation.Y - GetActorLocation().Y);

		float HalfWidth = MiniMapCapture->OrthoWidth * 0.5f;

		bool bInside =
			FMath::Abs(Delta.X) < HalfWidth &&
			FMath::Abs(Delta.Y) < HalfWidth;

		// 미니맵 안에 아이템이 있는지 확인
		if (bInside)
		{
			FVector2D MiniMapPos = WorldToMiniMap(ItemLocation);

			// 위젯에 전달
			MiniMapWidget->UpdateItemIcon(Item, MiniMapPos);

			// 로그를 찍어 아이템이 업데이트되는지 확인
			UE_LOG(LogTemp, Log, TEXT("Updating item %s on mini-map at position (%f, %f)"),
				*Item->GetName(), MiniMapPos.X, MiniMapPos.Y);
		}
		// 미니맵 밖에 아이템이 있으면 아이콘 숨기기
		else
		{
			MiniMapWidget->HideItemIcon(Item);

			UE_LOG(LogTemp, Log, TEXT("Hiding item %s from mini-map because it's outside the view"),
				*Item->GetName());
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