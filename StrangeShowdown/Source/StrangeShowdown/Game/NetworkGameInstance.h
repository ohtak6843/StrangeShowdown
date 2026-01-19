// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "NetworkGameInstance.generated.h"

class SocketIO;

/**
 * @brief: 서버와의 네트워크 연결을 담당하는 GameInstance
 */
UCLASS()
class STRANGESHOWDOWN_API UNetworkGameInstance : public UGameInstance
{
	GENERATED_BODY()



public:
	UFUNCTION(BlueprintCallable)
	void ConnectToGameServer();

	UFUNCTION(BlueprintCallable)
	void DisconnectFromGameServer();


public:
	FSocket* Socket{};
	TSharedPtr<SocketIO> SocketIOInstance{};

};
