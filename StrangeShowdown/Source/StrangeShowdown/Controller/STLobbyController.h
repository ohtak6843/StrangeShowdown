// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Controller/STBaseController.h"
#include "Interface/STControllerHUDInterface.h"
#include "STLobbyController.generated.h"

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API ASTLobbyController : public ASTBaseController, public ISTControllerHUDInterface
{
	GENERATED_BODY()
public:
	ASTLobbyController();

	virtual void SetupInputComponent() override;
	virtual class USTChatManagerWidget* GetChatManagerWidget() override;

	UPROPERTY(BlueprintReadWrite, Category = "Lobby")
	TObjectPtr<class USTLobbyHUD> LobbyHUDWidget;

	UPROPERTY(BlueprintReadOnly, Category = "Lobby")
	int currentPlayerCount = 1;
	int maxPlayerCount = 5;
	
protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	void SetReady();

	// 방장일 경우 새로운 플레이어가 들어오면 해당 함수를 호출
	void UpdateReadyText();

private:
	void AddInputAction();
	void FocusChatManager();
	bool bIsReady = false;
	bool bIsRoomOwner = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ReadyAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> FocusChatManagerAction;
};
