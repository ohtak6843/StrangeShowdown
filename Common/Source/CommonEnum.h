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
enum class PlayerType : uint8
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
enum class PlayerState : uint8
{
	None = 0,
	Aiming = 1 << 0,
	LookingUp = 1 << 1,
};

enum class ItemType : uint32
{
	None = 0,
	Pistol,
	Hammer,
	Helmet,
	Meat,
	Whiskey,
	EnhancePower,
	Letter,
	Wheel,
	End
};



COMMON_END