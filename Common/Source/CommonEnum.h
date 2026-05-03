#pragma once
#include <cstdint>

#include "CommonNamespace.h"


COMMON_START

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

enum class PlayerState : uint8_t
{
	None = 0,
	Idle = 1 << 0,
	Jumping = 1 << 1,
	Falling = 1 << 2,
	HoldItem = 1 << 3,
	Aiming = 1 << 4,
	LookingUp = 1 << 5,
	Dead = 1 << 6,
};

COMMON_END