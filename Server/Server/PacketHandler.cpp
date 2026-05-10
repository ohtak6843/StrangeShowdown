#include "pch.h"
#include "PacketHandler.h"
#include "RoomManager.h"
#include "IOCP.h"
#include "Serializer.h"

std::unordered_map<Common::PacketType, PacketHandler::HandlerFunc> PacketHandler::_handlerMap;

void PacketHandler::Init()
{
	// static 핸들러 등록

	RegisterHandler<Common::CSLogin>(
		Common::PacketType::CS_LOGIN,
		[](SessionPtr session, const auto& packet)
		{
			HandleLogin(session, packet);
		}
	);

	RegisterHandler<Common::CSMovePlayer>(
		Common::PacketType::CS_MOVE_PLAYER,
		[](SessionPtr session, const auto& packet)
		{
			HandleMovePlayer(session, packet);
		}
	);

	RegisterHandler<Common::CSGetRoomList>(
		Common::PacketType::CS_GET_ROOM_LIST,
		[](SessionPtr session, const auto& packet)
		{
			HandleGetRoomList(session, packet);
		}
	);

	RegisterHandler<Common::CSCreateRoom>(
		Common::PacketType::CS_CREATE_ROOM,
		[](SessionPtr session, const auto& packet)
		{
			HandleCreateRoom(session, packet);
		}
	);

	RegisterHandler<Common::CSJoinRoom>(
		Common::PacketType::CS_JOIN_ROOM,
		[](SessionPtr session, const auto& packet)
		{
			HandleJoinRoom(session, packet);
		}
	);

	RegisterHandler<Common::CSReady>(
		Common::PacketType::CS_READY,
		[](SessionPtr session, const auto& packet)
		{
			HandleReady(session, packet);
		}
	);

	RegisterHandler<Common::CSStartGame>(
		Common::PacketType::CS_START_GAME,
		[](SessionPtr session, const auto& packet)
		{
			HandleStartGame(session, packet);
		}
	);

	// dynamic handler 등록

	RegisterDynamicHandler<Common::CSChat>(
		Common::PacketType::CS_CHAT,
		[](SessionPtr session, const auto& packet, const uint8* payload, const uint16 payload_size)
		{
			HandleChat(session, packet, payload, payload_size);
		}
	);
}

void PacketHandler::HandlePacket(SessionPtr session, const RecvBuffer& buffer)
{
	if (nullptr == session)
	{
		return;
	}

	Common::Header& header{ *reinterpret_cast<Common::Header*>(const_cast<uint8*>(buffer.data())) };
	auto type{ header.type };
	auto iter{ _handlerMap.find(type) };
	if (_handlerMap.end() != iter)
	{
		auto& handler_func{ iter->second };
		handler_func(session, buffer);
	}
}

void PacketHandler::HandleMovePlayer(SessionPtr session, const Common::CSMovePlayer& packet)
{
	auto id{ session->GetSessionID() };
	if (IOState::INGAME != session->GetIOState())
	{
		return;
	}


	auto room{ session->GetRoom()};
	if (nullptr == room)
	{
		return;
	}

	const auto& player_map{ room->GetPlayers() };
	auto res{ player_map.find(id) };
	if (res == player_map.end())
	{
		return;
	}

	auto& player{ res->second };

	player->HandleMove(packet);

	 
	for (const auto& [other_id, other_player] : player_map)
	{
		if (other_id == id)
		{
			continue;
		}

		// 다른 플레이어에게 내 위치 전달
		Common::SCMovePlayer move_object_packet{
			id,
			player->GetPosition(),
			player->GetDirection(),
			player->GetAnimationState()
		};
		other_player->GetOwnerSession()->DoSend(move_object_packet);
	}
}

void PacketHandler::HandleLogin(SessionPtr session, const Common::CSLogin& packet)
{
	auto id{ session->GetSessionID() };
	Common::SCLogin login_packet{};
	session->DoSend(login_packet);
}

void PacketHandler::HandleGetRoomList(SessionPtr session, const Common::CSGetRoomList& packet)
{
	auto room_list{ GET_SINGLE(RoomManager)->GetRoomList() };

	Common::SCGiveRoomList room_list_packet{
		static_cast<uint16>(room_list.size())
	};
	room_list_packet.size += static_cast<uint16>(sizeof(Common::RoomInfo) * room_list.size());

	// sendbuffer에 패킷과 방 리스트를 직접 직렬화해서 보냄.
	auto buffer{ Serializer::Serialize(room_list_packet, room_list) };
	session->DoSend(buffer);
}

void PacketHandler::HandleCreateRoom(SessionPtr session, const Common::CSCreateRoom& packet)
{
	GET_SINGLE(RoomManager)->HandleCreateRoom(session, packet);
}

void PacketHandler::HandleJoinRoom(SessionPtr session, const Common::CSJoinRoom& packet)
{
	GET_SINGLE(RoomManager)->HandleJoinRoom(session, packet);
}

void PacketHandler::HandleReady(SessionPtr session, const Common::CSReady& packet)
{
	if (auto room{ session->GetRoom() }; nullptr != room)
	{
		room->HandleReady(session, packet);
	}
}

void PacketHandler::HandleStartGame(SessionPtr session, const Common::CSStartGame& packet)
{
	if (auto room{ session->GetRoom() }; nullptr != room)
	{
		room->HandleStart(session);
	}
}

void PacketHandler::HandleChat(SessionPtr session, const Common::CSChat& packet, const uint8* payload, const uint16 payload_size)
{
	if (auto room{ session->GetRoom() }; nullptr != room)
	{
		room->HandleChat(session, packet, payload, payload_size);
	}
}
