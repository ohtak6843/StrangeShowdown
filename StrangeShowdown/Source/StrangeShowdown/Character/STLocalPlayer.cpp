// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/STLocalPlayer.h"
#include "Item/STPickupItem.h"

ASTLocalPlayer::ASTLocalPlayer()
{
	PrimaryActorTick.bCanEverTick = true;

	// Spring Arm Component
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 300.0f;
	SpringArmComp->bUsePawnControlRotation = true;

	// Camera Component
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;

	// Inventory Component
	InventoryComp = CreateDefaultSubobject<USTInventoryComponent>(TEXT("InventoryComp"));

	// Stat Component
	StatComp = CreateDefaultSubobject<USTStatComponent>(TEXT("StatComp"));
	StatComp->Hp = StatComp->MaxHp;
	StatComp->Gold = 0;
	StatComp->Kill = 0;
	StatComp->MoveSpeed = 500;
	StatComp->Stamina = 5;
	StatComp->Action = 1;
	StatComp->Prize = 0;
	StatComp->bAlive = true;
}

void ASTLocalPlayer::BeginPlay()
{
	Super::BeginPlay();


}

void ASTLocalPlayer::Interact()
{
	FVector Start = CameraComp->GetComponentLocation();
	FVector ForwardVector = CameraComp->GetForwardVector();
	FVector End = ((ForwardVector * 500.f) + Start);
	
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	
	bool bIsHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams);
	if (bIsHit)
	{
		ASTPickupItem* PickupItem = Cast<ASTPickupItem>(HitResult.GetActor());
		if (PickupItem)
		{
			if (InventoryComp && PickupItem->ItemData)
			{
				bool bAdded = InventoryComp->AddItem(PickupItem->ItemData, 1);
				if (bAdded)
				{
					PickupItem->Destroy();
				}
			}
		}
	}
}