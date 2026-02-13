#include "pch.h"
#include "Player.h"

void Player::HandleMove(const Common::CSMovePlayer& packet)
{
	_position = packet.pos;
	_direction = packet.dir;
	_state = packet.state;
}
