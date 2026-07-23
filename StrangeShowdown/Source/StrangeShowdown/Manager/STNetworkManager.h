// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "STNetworkManager.generated.h"


class SocketIO;
class STPacketHandler;

/**
 * brief:
 *  network 관련 기능을 담당하는 매니저 클래스
 */
UCLASS()
class STRANGESHOWDOWN_API USTNetworkManager : public UObject
{
	GENERATED_BODY()

	// --
	// lifecycle
	// --
public:
	void Start();
	virtual void BeginDestroy() override;

	// --
	// network method
	// --
private:
	void ConnectToGameServer();
	void DisconnectFromGameServer();
	void HandleRecvPackets();
	bool Tick(float DeltaTime);

public:
	void SendPacket(const TArray<uint8>& Packet);
	
	void Pause() { IsLoadingLevel = true; };
	void Resume() { IsLoadingLevel = false; };


private:
	FSocket* Socket{};
	TSharedPtr<SocketIO> SocketIOInstance{};
	TSharedPtr<STPacketHandler> PacketHandler{};
	// 레벨 로딩 중 여부
	bool IsLoadingLevel{ false };
	FTSTicker::FDelegateHandle TickHandle{};
	
};
