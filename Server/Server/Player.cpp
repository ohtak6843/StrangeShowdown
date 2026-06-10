#include "pch.h"
#include "Player.h"
#include "Room.h"

void Player::Init(const std::shared_ptr<Room>& room, const SessionPtr session)
{
	// 초기화
	Clear();

	if (nullptr != room)
	{
		_room = room;
	}
	else
	{
		// todo: 예외 처리
		std::println("Error: Player::Init called with nullptr room");
	}

	if (nullptr != session)
	{
		_ownerSession = session;
	}
	else
	{
		// todo: 예외 처리
		std::println("Error: Player::Init called with nullptr session");
	}
	
	// 캐릭터 배치
	ChangeType(Common::PlayerType::LobbyPlayer);
}

void Player::Clear()
{
	_position = {};
	_direction = {};
	_animationState = 0;
	_ready = false;
	_inventory.fill(0);
	_status = {};
}

void Player::ChangeType(const Common::PlayerType type)
{
	Clear();

	_type = type;
	_status.Init(type);

	// 총과 망치는 기본 지급
	if (Common::PlayerType::Player == type)
	{
		_inventory[static_cast<size_t>(Common::ItemType::Pistol)] = 1;
		_inventory[static_cast<size_t>(Common::ItemType::Hammer)] = 1;
	}
}

bool Player::TryConsumeItem(const Common::ItemType item_type)
{
	// 예외 처리
	if (item_type <= Common::ItemType::None || item_type >= Common::ItemType::End)
	{
		return false;
	}
	
	int item_delta{ -1 };
	float stamina_delta{ -Common::ItemConstants::Stamina[static_cast<size_t>(item_type)] };

	// 아이템 개수 확인

	// 비소모성 아이템 
	if (Common::ItemType::Pistol == item_type ||
		Common::ItemType::Hammer == item_type)
	{
		item_delta = 0;
	}
	// 소모성 아이템
	else
	{
		if (0 == _inventory[static_cast<size_t>(item_type)])
		{
			return false;
		}
	}

	// 스테미나 확인
	if (_status.stamina + stamina_delta < -std::numeric_limits<float>::epsilon())
	{
		return false;
	}

	// 기타 조건 확인



	// 여기까지 왔으면 아이템 사용 가능
	// 아이템 사용 처리
	// 주의! 효과 적용은 별개임.
	_inventory[static_cast<size_t>(item_type)] += item_delta;
	_status.stamina += stamina_delta;

	return true;
}

void Player::ApplyItemEffect(const Common::CSUseItem packet, const PlayerPtr target)
{
	// 예외처리
	auto room{ _room.lock() };
	if (nullptr == room)
	{
		return;
	}

	// 아이템 사용 패킷을 본인 및 다른 플레이어에게 전달.

	// 다른 플레이어에게 전달하는 아이템 사용 패킷
	// 사용자의 보유 아이템 개수를 공개하지 않음
	Common::SCUseItem use_item_packet_other{
		_ownerSession->GetSessionID(),
		packet.targetID,
		packet.itemType,
	};
	room->Broadcast(use_item_packet_other, _ownerSession->GetSessionID());

	// 사용자 플레이어에게 전달하는 아이템 사용 패킷
	// 사용자의 보유 아이템 개수를 공개함
	Common::SCUseItem use_item_packet_self{
		_ownerSession->GetSessionID(),
		packet.targetID,
		packet.itemType,
	};
	_ownerSession->DoSend(use_item_packet_self);

	// 아이템 효과 적용 및 사용 효과 패킷 전달
	switch (packet.itemType)
	{
		case Common::ItemType::Pistol:
		{
			if (nullptr != target)
			{
				target->TakeDamage(_status.attack);
			}

			const auto& target_status{ target->GetStatus() };
			Common::SCStatusUpdate status_update_packet{
				target->GetOwnerSession()->GetSessionID(),
				target_status.hp,
				target_status.stamina,
				target_status.bullet,
				target_status.gold,
				target_status.armor
			};
			room->Broadcast(status_update_packet);

		}
		break;

		case Common::ItemType::Hammer:
		{
			// 망치로 문 부수기
		}
		break;

		case Common::ItemType::Helmet:
		{
			_status.armor += Common::ItemConstants::HelmetValue;
			
		}
		break;

		case Common::ItemType::Meat:
		{
			_status.hp += Common::ItemConstants::MeatValue;
		}
		break;

		case Common::ItemType::Whiskey:
		{
			_status.stamina += Common::ItemConstants::WhiskeyValue;
		}
		break;

		case Common::ItemType::EnhancePower:
		{
			// 매 턴마다 총알 1 제공
			
		}
		break;

		case Common::ItemType::Letter:
		{
			// 무작위 미션 추가
			
		}
		break;

		case Common::ItemType::Wheel:
		{
			// 이동속도 증가 ( 클라이언트에서 처리 ) 
			
		}
		break;

	default:
		break;
	}

	// 스탯 업데이트 패킷 전달
	Common::SCStatusUpdate status_update_packet{
		_ownerSession->GetSessionID(),
		_status.hp,
		_status.stamina,
		_status.bullet,
		_status.gold,
		_status.armor
	};
	room->Broadcast(status_update_packet);

}

void Player::TakeDamage(const float damage)
{
	_status.hp -= damage;

	// 체력이 0이 되었을떄 유령 플레이어 생성
	if (_status.hp <= std::numeric_limits<float>::epsilon())
	{
		ChangeType(Common::PlayerType::Ghost);
		
		// 기존 플레이어 제거 패킷 전송
		Common::SCDespawnPlayer despawn_packet{ _ownerSession->GetSessionID() };
		auto room{ _room.lock() };
		if (nullptr != room)
		{
			room->Broadcast(despawn_packet);
		}

		// 새로이 유령 플레이어 생성 패킷 전송
		Common::SCSpawnPlayer spawn_packet{
			_ownerSession->GetSessionID(),
			_position,
			_direction,
			_type
		};
		if (nullptr != room)
		{
			room->Broadcast(spawn_packet);
		}
	}
}

void Player::HandleMove(const Common::CSMovePlayer& packet)
{
	_position = packet.pos;
	_direction = packet.dir;
	_animationState = packet.state;
}

void Player::HandlePickItem(const Common::CSPickItem& packet)
{
	// todo: 검증
	++_inventory[static_cast<size_t>(packet.itemType)];
	std::println("Player {} picked up item {}, new count: {}", _ownerSession->GetSessionID(), static_cast<int>(packet.itemType), _inventory[static_cast<size_t>(packet.itemType)]);
}

int Player::GetItemCount(const Common::ItemType item_type) const
{
	if (item_type <= Common::ItemType::None || item_type >= Common::ItemType::End)
	{
		return 0;
	}
	return _inventory[static_cast<size_t>(item_type)];
}
