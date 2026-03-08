// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/STCharacter.h"
#include "Actor/STInteractableActor.h"
#include "Components/WidgetComponent.h"
#include "STStorekeeper.generated.h"

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API ASTStorekeeper : public ASTCharacter, public IInteractable
{
	GENERATED_BODY()

public:
	ASTStorekeeper();

	virtual void Interact_Implementation(APawn* Interactor) override;

	// Interact UI À§Á¬
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI", Meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* InteractWidgetComponent;

protected:
	virtual void BeginPlay() override;

private:
	
};
