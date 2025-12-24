// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/STStoreComponent.h"
#include "Component/STStatComponent.h"
#include "Component/STInventoryComponent.h"

// Sets default values for this component's properties
USTStoreComponent::USTStoreComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USTStoreComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerPlayer = Cast<ASTLocalPlayer>(GetOwner());

	// CurrentItem 초기화
	CurrentItems.SetNum(5);

	InitializeStore();
}

void USTStoreComponent::InitializeStore()
{
	// 여기서 아이템 데이터 에셋을 서버로부터 받아와서
	// 블루프린트에서 이미지와 가격을 설정해야 함

	// 임시로 모든 아이템에 0번 아이템 할당
	if (StoreItemPool.Num() == 0)
		return;

	for (int32 i = 0; i < CurrentItems.Num(); i++)
	{
		CurrentItems[i].ItemData = StoreItemPool[0];
		CurrentItems[i].Price = CurrentItems[i].ItemData->GoldCost;
		CurrentItems[i].bSold = false;
	}
}

bool USTStoreComponent::BuyItem(int32 Index)
{
	// 플레이어 골드 차감
	OwnerPlayer->StatComp->AddGold(-CurrentItems[Index].Price);

	// 아이템 구매 처리
	CurrentItems[Index].bSold = true;

	// 아이템 인벤토리로 이동
	OwnerPlayer->InventoryComp->AddItem(CurrentItems[Index].ItemData, 1);

	return false;
}

void USTStoreComponent::Reroll()
{
	// 플레이어 골드 차감
	OwnerPlayer->StatComp->AddGold(-RerollCost);

	// 상점 초기화
	InitializeStore();
}