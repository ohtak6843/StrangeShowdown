#include "pch.h"
#include "PacketHandler.h"
#include "RoomManager.h"
#include "IOCP.h"
#include "Serializer.h"

std::unordered_map<packet::Type, PacketHandler::HandlerFunc> PacketHandler::_handlerMap;

void PacketHandler::Init()
{
	// 핸들러 등록
	RegisterHandler<packet::CSLogin>(
		packet::Type::CS_LOGIN,
		[](SessionPtr session, const auto& pkt) {
			HandleLogin(session, pkt);
		}
	);

	RegisterHandler<packet::CSMovePlayer>(
		packet::Type::CS_MOVE_PLAYER,
		[](SessionPtr session, const auto& pkt) {
			HandleMovePlayer(session, pkt);
		}
	);
}

void PacketHandler::HandlePacket(SessionPtr session, const RecvBuffer& buffer)
{
	if (nullptr == session)
	{
		return;
	}

	auto type{ static_cast<packet::Type>(buffer[1]) };
	auto iter{ _handlerMap.find(type) };
	if (_handlerMap.end() != iter)
	{
		auto& handler_func{ iter->second };
		handler_func(session, buffer);
	}
}

void PacketHandler::HandleMovePlayer(SessionPtr session, const packet::CSMovePlayer& packet)
{
	auto id{ session->GetSessionID() };
	auto player{ GET_SINGLE(RoomManager)->GetPlayer(id) };

	player->_position = packet.pos;
	player->_direction = packet.dir;

	// todo: 변경: _sessionHash 직접 접근에서 RoomManager 통해 접근
	// 다른 플레이어에게도 전파
	auto begin_iter{ GET_SINGLE(IOCP)->_sessionHash.begin() };
	auto end_iter{ GET_SINGLE(IOCP)->_sessionHash.end() };
	for (; begin_iter != end_iter; ++begin_iter)
	{
		auto other_session{ begin_iter->second };
		if (other_session->GetSessionID() == id)
		{
			continue;
		}

		// todo: 이부분을 수정
		packet::SCMoveObject move_object_packet{
			id,
			player->_position,
			player->_direction
		};
		other_session->DoSend(move_object_packet);
	}
}

void PacketHandler::HandleLogin(SessionPtr session, const packet::CSLogin& packet)
{
	auto id{ session->GetSessionID() };

	auto player{ GET_SINGLE(RoomManager)->GetPlayer(id) };


	// todo: 변경: _sessionHash 직접 접근에서 RoomManager 통해 접근
	// 다른 플레이어에게도 전파
	auto begin_iter{ GET_SINGLE(IOCP)->_sessionHash.begin() };
	auto end_iter{ GET_SINGLE(IOCP)->_sessionHash.end() };
	for (; begin_iter != end_iter; ++begin_iter)
	{
		auto other_session{ begin_iter->second };
		auto other_player{ GET_SINGLE(RoomManager)->GetPlayer(
			other_session->GetSessionID())
		};
		if (other_session->GetSessionID() == id)
		{
			continue;
		}

		// todo: 아래 부분을 수정

		// 다른 플레이어에게 새로 접속한 플레이어 정보 전파
		packet::SCSpawnObject move_object_packet{
			id,
			player->_position,
			player->_direction
		};
		other_session->DoSend(move_object_packet);

		// 현재 클라이언트에 기존 플레이어 정보 전달
		packet::SCSpawnObject other_spawn_packet{
			other_session->GetSessionID(),
			other_player->_position,
			other_player->_direction
		};
		session->DoSend(other_spawn_packet);
	}
}
