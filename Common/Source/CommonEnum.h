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
	Aiming = 1 << 0,
	LookingUp = 1 << 1,
};

COMMON_END