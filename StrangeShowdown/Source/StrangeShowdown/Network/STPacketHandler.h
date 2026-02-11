// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StrangeShowdown.h"
#include "Network/STSerializer.h"
#include "Game/STGameInstance.h"

/**
 * Socket I/O 통신
 */
class STRANGESHOWDOWN_API STPacketHandler
{
public:
	STPacketHandler();
	~STPacketHandler();

	void HandlePacket(const TArray<uint8>& Data);

private:
	void HandleSpawnObject(const packet::SCSpawnObject& Pkt);
	void HandleMoveObject(const packet::SCMoveObject& Pkt);


	// 핸들러 맵에 함수 등록
	template <typename T>
	void RegisterHandler(packet::Type Type, TFunction<void(const T&)> LogicFunc)
	{
		HandlerMap.Add(Type, [LogicFunc](const TArray<uint8>& Data) {
			T Pkt{ STSerializer::Deserialize<T>(Data) };
			LogicFunc(Pkt);
		});
	}
	

private:

	// 현재 인스턴스
	UPROPERTY()
	TObjectPtr<USTGameInstance> GameInstance;

	// 핸들러 함수 맵
	using FPacketHandlerFunc = TFunction<void(const TArray<uint8>&)>;
	TMap<packet::Type, FPacketHandlerFunc> HandlerMap;

};