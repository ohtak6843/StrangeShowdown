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
	HoldItem = 1 << 2,
	ArmedPistol = 1 << 3,
	ArmedHammer = 1 << 4,
	Aiming = 1 << 5,
	LookingUp = 1 << 6,
	Dead = 1 << 7,
};

COMMON_END