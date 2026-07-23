#pragma once
#include <cstdint>

#include "CommonNamespace.h"


COMMON_START

/*
* brief:
*  플레이어의 외형을 나타내는 enum class
*/
enum class PlayerApperance : uint8
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
*  플레이어 현재 타입을 나타내는 enum class
*/
enum class PlayerType : uint8
{
	None = 0,
	LobbyPlayer, // 로비 상태의 플레이어
	Player, // 인게임 상태의 플레이어
	Ghost,  // 인게임 중 죽어 유령이 된 플레이어
	Sheriff, // 플레이어가 조종하고 있는 보안관
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


enum class ObjectType : uint8
{
	None = 0,
	Player,
	Sheriff,
	PickUpUItem
};



COMMON_END