// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/STCharacter.h"
#include "Actor/STInteractableActor.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "Interface/STMiniMapTargetInterface.h"
#include "STStorekeeper.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterEnter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterExit);

UCLASS()
class STRANGESHOWDOWN_API ASTStorekeeper : public ASTCharacter, public IInteractable, public ISTMiniMapTargetInterface
{
	GENERATED_BODY()

public:
	ASTStorekeeper();

	virtual void Interact_Implementation(APawn* Interactor) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Store")
	TArray<class USTItemDataAssetBase*> StoreItemPool;

	// Interact UI 위젯
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI", Meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* InteractWidgetComponent;

	// 콜리전
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Interact")
	USphereComponent* InteractCollision;

	// Overlap 이벤트
	UPROPERTY(BlueprintAssignable)
	FOnCharacterEnter OnPlayerEnter;

	UPROPERTY(BlueprintAssignable)
	FOnCharacterExit OnPlayerExit;

	UFUNCTION()
	void BuyItem(int32 SlotIndex);

	UFUNCTION(BlueprintCallable)
	void UnregisterStore(USTStoreComponent* StoreComp);

	UFUNCTION(BlueprintCallable)
	void UpdateOpenedStores();

	// 아이템과 오버랩 중인 플레이어 저장
	UPROPERTY()
	ASTCharacter* OverlappedPlayer = nullptr;

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION()
	void HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void HandleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// Carriage
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UChildActorComponent> Carriage;

	UPROPERTY()
	APlayerCameraManager* CachedCameraManager;

	// 상점을 열고 있는 플레이어(의 상점 컴포넌트)
	UPROPERTY()
	TArray<USTStoreComponent*> OpenedStores;

	// 미니맵에 보이는 아이콘
	TObjectPtr<UTexture2D> MiniMapIcon;

public:
	// ISTMiniMapTargetInterface 구현
	virtual FVector GetMiniMapLocation_Implementation() const;
	virtual UTexture2D* GetMiniMapIcon_Implementation() const;
};
