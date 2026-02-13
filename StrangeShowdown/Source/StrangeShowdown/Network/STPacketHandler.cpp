// Fill out your copyright notice in the Description page of Project Settings.


#include "Network/STPacketHandler.h"

STPacketHandler::STPacketHandler()
{
	
	GameInstance = Cast<USTGameInstance>(GWorld->GetGameInstance());

	RegisterHandler<Common::SCSpawnObject>(
		Common::PacketType::SC_SPAWN_OBJECT, [this](const auto& Pkt) {
			HandleSpawnObject(Pkt);
		}
	);

	RegisterHandler<Common::SCMoveObject>(
		Common::PacketType::SC_MOVE_OBJECT, [this](const auto& Pkt) {
			HandleMoveObject(Pkt);
		}
	);
}

STPacketHandler::~STPacketHandler()
{
}

void STPacketHandler::HandlePacket(const TArray<uint8>& Data)
{
	auto Type{ static_cast<Common::PacketType>(Data[1]) };
	if (auto* Func{ HandlerMap.Find(Type) })
	{
		(*Func)(Data);
	}
}

void STPacketHandler::HandleSpawnObject(const Common::SCSpawnObject& Pkt)
{
	if (nullptr == GameInstance)
	{
		return;
	}
	GameInstance->HandleSpawn(Pkt);
}

void STPacketHandler::HandleMoveObject(const Common::SCMoveObject& Pkt)
{
	if (nullptr == GameInstance)
	{
		return;
	}
	GameInstance->HandleMove(Pkt);
}
