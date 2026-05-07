#pragma once
#include <cstdint>

#include "CommonNamespace.h"


COMMON_START

// --
// player
// --

/*
* brief:
*  플레이어의 외형을 나타내는 enum class
*/
enum class PlayerType : uint8_t
{
	Badguy,
	BuisinessMan,
	Cowboy,
	Cowgirl,
	Gunman,
	Sheriff,
	Woman,
	WorkingGirl,
};

/*
* brief:
*  플레이어의 현재 상태를 나타내는 enum class
*  주로 Ingame에서 사용됨.
*  bit flag로 구현되어, 여러 상태가 동시에 적용될 수 있음.
*/
enum class PlayerState : uint8_t
{
	None = 0,
	Idle = 1 << 0,
	Jumping = 1 << 1,
	HoldItem = 1 << 2,
	ArmedPistol = 1 << 3,
	ArmedHammer = 1 << 4,
	Aiming = 1 << 5,
	LookingUp = 1 << 6,
	Dead = 1 << 7,
};


COMMON_END