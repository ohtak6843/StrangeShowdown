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

FVector2D ASTMiniMapActor::WorldToMiniMap(const FVector& ItemLocation, const FVector& PlayerLocation, float PlayerYaw) const
{
	FVector2D Relative(
		ItemLocation.X - PlayerLocation.X,
		ItemLocation.Y - PlayerLocation.Y);

	// 플레이어 회전에 따라 상대 위치 회전
	Relative = Relative.GetRotated(-PlayerYaw);

	// 스케일 조정
	float Scale = 0.05f;
	Relative *= Scale;
	
	// 위젯 중심 맞추기
	float WidgetSize = 200.f;
	Relative += FVector2D(WidgetSize * 0.5f, WidgetSize * 0.5f);

	return Relative;
}

void ASTMiniMapActor::UpdateItemOnMiniMap(float DeltaTime)
{
	if (!MiniMapWidget) return;
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;
	APawn* PlayerPawn = PC->GetPawn();
	if (!PlayerPawn) return;

	for (ASTPickupItem* Item : MiniMapItems)
	{
		if (!Item) continue;
		FVector2D MiniMapPos = WorldToMiniMap(Item->GetActorLocation(), PlayerPawn->GetActorLocation(), PC->GetControlRotation().Yaw);

		MiniMapWidget->UpdateItemIcon(Item, MiniMapPos);

		// MiniMapPos Log
		UE_LOG(LogTemp, Log, TEXT("Item: %s, World Location: %s, MiniMap Position: %s"),
			*Item->GetName(),
			*Item->GetActorLocation().ToString(),
			*MiniMapPos.ToString());
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