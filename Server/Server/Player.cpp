#include "pch.h"
#include "Player.h"

void Player::Init(const PlayerState state)
{
	_state = state;
	_status.Init(state);
}

void Player::HandleMove(const Common::CSMovePlayer& packet)
{
	_position = packet.pos;
	_direction = packet.dir;
	_animationState = packet.state;
}
