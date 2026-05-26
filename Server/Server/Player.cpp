#include "pch.h"
#include "Player.h"

void Player::Init(const PlayerState state)
{
	_state = state;
	_status.Init(state);
	
	// 총과 망치는 기본 지급
	_inventory[1] = 1;
	_inventory[2] = 1;
	std::ranges::fill(_inventory, 100);
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
	if (_status.stamina + stamina_delta < -1e-6f)
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

void Player::HandleMove(const Common::CSMovePlayer& packet)
{
	_position = packet.pos;
	_direction = packet.dir;
	_animationState = packet.state;
}

int Player::GetItemCount(const Common::ItemType item_type) const
{
	if (item_type <= Common::ItemType::None || item_type >= Common::ItemType::End)
	{
		return 0;
	}
	return _inventory[static_cast<size_t>(item_type)];
}
