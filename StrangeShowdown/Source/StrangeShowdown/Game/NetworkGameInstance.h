// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "Containers/Map.h"
#include "StrangeShowdown.h"
#include "Player/STPlayerBase.h"
#include "Player/STFieldPlayer.h"

#include "NetworkGameInstance.generated.h"

class SocketIO;
class STPacketHandler;

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

	UFUNCTION(BlueprintCallable)
	void HandleRecvPackets();


	void HandleSpawn(const packet::SCSpawnObject& SpawnPacket);
	void HandleMove(const packet::SCMoveObject& MovePacket);

	void SendPacket(const TArray<uint8>& data);

public:


public:
	FSocket* Socket{};
	TSharedPtr<SocketIO> SocketIOInstance{};
	TSharedPtr<STPacketHandler> PacketHandler{};

	// 어떤 블프 클래스인지 알아야 함.
	UPROPERTY(EditAnywhere, Category = "SpawnData")
	TSubclassOf<ASTFieldPlayer> OtherPlayerClass;

	TMap<uint64, ASTFieldPlayer*> PlayerMap;
};
