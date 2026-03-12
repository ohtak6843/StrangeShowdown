// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/STBigMapActor.h"
#include "Kismet/GameplayStatics.h"
#include "Components/WidgetComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Controller/STBaseController.h"
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

	SetActorLocation(FVector(0, -3500.f, zPosition));

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

	UpdateItemOnMiniMap(DeltaTime);

	UpdatePlayerOnMiniMap(DeltaTime);
}

void ASTBigMapActor::BringHUD()
{
	ASTBaseController* STPC = Cast<ASTBaseController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
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

	MiniMapWidget->SetIsRotationAble(false);
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

void ASTBigMapActor::UpdateItemOnMiniMap(float DeltaTime)
{
	if (!MiniMapWidget) return;

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC) return;

	APawn* PlayerPawn = PC->GetPawn();
	if (!PlayerPawn) return;

	float CurrentYaw = GetActorRotation().Yaw;

	for (ASTPickupItem* Item : MiniMapItems)
	{
		if (!Item) continue;

		FVector2D MiniMapPos =
			WorldToMiniMap(Item->GetActorLocation());

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