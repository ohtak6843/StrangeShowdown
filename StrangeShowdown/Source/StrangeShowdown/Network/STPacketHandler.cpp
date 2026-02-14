// Fill out your copyright notice in the Description page of Project Settings.


#include "Network/STPacketHandler.h"

STPacketHandler::STPacketHandler()
{
	
	GameInstance = Cast<USTGameInstance>(GWorld->GetGameInstance());

	RegisterHandler<Common::SCSpawnObject>(
		Common::PacketType::SC_SPAWN_OBJECT, [this](const auto& Packet) {
			HandleSpawnObject(Packet);
		}
	);

	RegisterHandler<Common::SCMovePlayer>(
		Common::PacketType::SC_MOVE_PLAYER, [this](const auto& Packet) {
			HandleMoveObject(Packet);
		}
	);

	RegisterHandler<Common::SCGiveRoomList>(
		Common::PacketType::SC_GIVE_ROOM_LIST, [this](const auto& Packet) {
			HandleGiveRoomList(Packet);
		}
	);
}

STPacketHandler::~STPacketHandler()
{
}

void STPacketHandler::HandlePacket(const TArray<uint8>& Data)
{
	Common::Header& header{ *reinterpret_cast<Common::Header*>(const_cast<uint8*>(Data.GetData())) };
	auto Type{ static_cast<Common::PacketType>(header.type) };
	if (auto* Func{ HandlerMap.Find(Type) })
	{
		(*Func)(Data);
	}
}

void STPacketHandler::HandleSpawnObject(const Common::SCSpawnObject& Packet)
{
	if (nullptr == GameInstance)
	{
		return;
	}
	GameInstance->HandleSpawn(Packet);
}

void STPacketHandler::HandleMoveObject(const Common::SCMovePlayer& Packet)
{
	if (nullptr == GameInstance)
	{
		return;
	}
	GameInstance->HandleMove(Packet);
}

void STPacketHandler::HandleGiveRoomList(const Common::SCGiveRoomList& Packet)
{
	if (nullptr == GameInstance)
	{
		return;
	}
	// GameInstance->HandleGiveRoomList(Packet);
	UE_LOG(LogTemp, Log, TEXT("Room Get Success. count: %d"), Packet.roomCount);
}
