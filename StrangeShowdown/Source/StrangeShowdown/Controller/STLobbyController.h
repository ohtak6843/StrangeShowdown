// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Controller/STBaseController.h"
#include "Interface/STControllerHUDInterface.h"
#include "protocol.h"
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
	
	// 클라이언트가 방 입장 시 데이터 초기화
	void InitData(const uint64 InHostPlayerID, const uint64 InMyPlayerID);

	// 외부 플레이어의 ready 상태 변경
	void SetOtherPlayerReady(const uint64 InPlayerID, const bool InReady);

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	// 본인 플레이어의 ready 상태 변경
	void SetReady();


	// 방장일 경우 새로운 플레이어가 들어오면 해당 함수를 호출

public:
	void UpdateReadyText();

private:
	void AddInputAction();
	void FocusChatManager();
	bool bIsReady = false;


private:
	bool bIsRoomOwner = false;
	uint64 MyPlayerID = 0;
	uint64 HostPlayerID = 0;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ReadyAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> FocusChatManagerAction;
};
