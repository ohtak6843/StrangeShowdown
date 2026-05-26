// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/STAreaVolume.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "Controller/STPlayerController.h"
#include "UI/AreaVolume/STAreaVolumeWidget.h"
#include "Character/Player/STLocalPlayer.h"
#include "UI/STHUDWidget.h"

// Sets default values
ASTAreaVolume::ASTAreaVolume()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	RootComponent = Box;

	Box->SetCollisionProfileName(TEXT("Trigger"));
}

// Called when the game starts or when spawned
void ASTAreaVolume::BeginPlay()
{
	Super::BeginPlay();
	
	Box->OnComponentBeginOverlap.AddDynamic(this, &ASTAreaVolume::OnOverlapBegin);
	Box->OnComponentEndOverlap.AddDynamic(this, &ASTAreaVolume::OnOverlapEnd);

	// HUD 연결 타이머 설정(딜레이)
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		this,
		&ASTAreaVolume::BringHUD,
		0.5f,
		false);
}

void ASTAreaVolume::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	// STLocalPlayer와 충돌했는지 확인
	if (OtherActor && OtherActor->IsA(ASTLocalPlayer::StaticClass()))
	{
		if (HUDWidget)
		{
			HUDWidget->GetAreaVolumeWidget()->UpdateAreaInfo(AreaType);
		}
	}
}

void ASTAreaVolume::OnOverlapEnd(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	// STLocalPlayer와 충돌이 끝났는지 확인
	if (OtherActor && OtherActor->IsA(ASTLocalPlayer::StaticClass()))
	{
		if (HUDWidget)
		{
			HUDWidget->GetAreaVolumeWidget()->DeleteAreaInfo(AreaType);
		}
	}
}

void ASTAreaVolume::BringHUD()
{
	ASTPlayerController* STPC = Cast<ASTPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (STPC)
	{
		HUDWidget = STPC->GetHUDWidget();
		if (!HUDWidget)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to get HUDWidget from PlayerController"));
			return;
		}
	}
}