#pragma once

#include "CommonNamespace.h"
#include "CommonEnum.h"
#include <string_view>

COMMON_START

// --
// network
// --
constexpr int PORT_NUMBER{ 7777 };
constexpr const char* SERVER_IP{ "127.0.0.1" };

constexpr float SendMoveTime{ 0.08f }; 


// --
// player
// --

namespace PlayerConstants
{
	//constexpr float MoveSpeed{ 5.f };
	const std::string_view	Name{ "NoName" };
	constexpr PlayerType	Type{ PlayerType::Badguy };
	constexpr float			MaxHp{ 100.f };
	constexpr float  		Hp{ 100.f };
	constexpr float 		MaxStamina{ 5.f };
	constexpr float 		Stamina{ 1.f };
	constexpr float 		MaxBullet{};
	constexpr float 		Bullet{};
	constexpr float 		Attack{ 10.f };
	constexpr float 		Speed{ 5.f };
	constexpr float 		Gold{ 0.f };

}

namespace ItemConstants
{
	// 스테미나 소모량
	constexpr std::array<float, static_cast<size_t>(Common::ItemType::End)> Stamina{
		0.f,	// None
		0.f,    // Gun
		1.f,    // Hammer
		2.f,    // HealPack
	};

}

// --
// room
// --

constexpr uint32 MaxPlayerCount{ 5 };


COMMON_END