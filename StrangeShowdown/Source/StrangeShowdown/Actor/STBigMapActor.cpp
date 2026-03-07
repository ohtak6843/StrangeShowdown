// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/STBigMapActor.h"
#include "Kismet/GameplayStatics.h"
#include "Components/WidgetComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Controller/STPlayerController.h"
#include "Widget/STMiniMapWidget.h"
#include "GameFramework/HUD.h"
#include "Widget/STHUD.h"
#include "Item/STPickupItem.h"

// Sets default values
ASTBigMapActor::ASTBigMapActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	MiniMapCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("MiniMapCapture"));
	MiniMapCapture->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ASTBigMapActor::BeginPlay()
{
	Super::BeginPlay();

	MiniMapCapture->ShowFlags.SetLighting(false);
	MiniMapCapture->ShowFlags.SetShadowFrustums(false);
	MiniMapCapture->ShowFlags.SetDynamicShadows(false);
	MiniMapCapture->ShowFlags.SetPostProcessing(false);

	// 위젯 컴포넌트 숨기기
	HiddenWidgetComponent();

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

	UpdateMiniMapRotation(DeltaTime);

	UpdateItemOnMiniMap(DeltaTime);
}

void ASTBigMapActor::CollectItems()
{
	MiniMapItems.Empty();

	TArray<AActor*> ItemActors;
	UGameplayStatics::GetAllActorsOfClass(
		GetWorld(),
		ASTPickupItem::StaticClass(),
		ItemActors);

	for (AActor* Actor : ItemActors)
	{
		ASTPickupItem* Item = Cast<ASTPickupItem>(Actor);
		if (Item)
		{
			MiniMapItems.Add(Item);

			// 파괴 이벤트 바인딩
			Item->OnDestroyed.AddDynamic(this, &ASTBigMapActor::OnItemDestroyed);
		}
	}
}

void ASTBigMapActor::BringHUD()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		ASTPlayerController* STPC = Cast<ASTPlayerController>(PC);
		if (STPC)
		{
			HUDWidget = STPC->HUDWidget;
			MiniMapWidget = HUDWidget->GetBigMapWidget();
			if (!MiniMapWidget)
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to get BigMapWidget from HUD"));
				return;
			}
		}
	}
}

void ASTBigMapActor::RegisterItem(ASTPickupItem* NewItem)
{
	if (!NewItem) return;

	MiniMapItems.AddUnique(NewItem);

	NewItem->OnDestroyed.AddDynamic(this, &ASTBigMapActor::OnItemDestroyed);
}

void ASTBigMapActor::OnItemDestroyed(AActor* DestroyedActor)
{
	ASTPickupItem* Item = Cast<ASTPickupItem>(DestroyedActor);
	if (!Item) return;

	MiniMapItems.Remove(Item);

	if (MiniMapWidget)
	{
		MiniMapWidget->HideItemIcon(Item);
	}
}

FVector2D ASTBigMapActor::WorldToMiniMap(const FVector& ItemLocation, const FVector& PlayerLocation, float PlayerYaw) const
{
	// 플레이어 기준 상대 위치(월드와 미니맵 좌표계는 XY축이 반대)
	FVector2D Relative(
		ItemLocation.Y - PlayerLocation.Y,
		ItemLocation.X - PlayerLocation.X);

	Relative.Y *= -1.f;

	// 미니맵 회전 적용
	float CurrentYaw = GetActorRotation().Yaw;
	Relative = Relative.GetRotated(-CurrentYaw);

	const float WidgetSize = HUDWidget->BigMapWidget->GetDesiredSize().Y;
	const float OrthoWidth = MiniMapCapture->OrthoWidth;

	const float Scale = WidgetSize / OrthoWidth;
	Relative *= Scale;

	const float HalfSize = WidgetSize * 0.5f;
	Relative += FVector2D(HalfSize, HalfSize);

	return Relative;
}

void ASTBigMapActor::UpdateItemOnMiniMap(float DeltaTime)
{
	if (!MiniMapWidget) return;

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;

	APawn* PlayerPawn = PC->GetPawn();
	if (!PlayerPawn) return;

	float CurrentYaw = GetActorRotation().Yaw;

	for (ASTPickupItem* Item : MiniMapItems)
	{
		if (!Item) continue;

		FVector2D MiniMapPos =
			WorldToMiniMap(
				Item->GetActorLocation(),
				PlayerPawn->GetActorLocation(),
				CurrentYaw);

		// TODO: 아이템이 밀리는 현상 존재, 임시 오프셋을 적용
		FVector2D Offset(-55.f, -20.f);
		MiniMapPos += Offset;

		const float WidgetSize = HUDWidget->BigMapWidget->GetDesiredSize().Y;

		const float MinX = Offset.X + 10.f;
		const float MaxX = Offset.X + WidgetSize - 15.f;
		const float MinY = Offset.Y + 10.f;
		const float MaxY = Offset.Y + WidgetSize - 10.f;

		if (MiniMapPos.X < MinX || MiniMapPos.X > MaxX || MiniMapPos.Y < MinY || MiniMapPos.Y > MaxY)
		{
			MiniMapWidget->HideItemIcon(Item);
		}
		else
		{
			MiniMapWidget->UpdateItemIcon(Item, MiniMapPos);
		}
	}
}

void ASTBigMapActor::UpdateMiniMapRotation(float DeltaTime)
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

void ASTBigMapActor::HiddenWidgetComponent()
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