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
	constexpr float 		Speed{ 500.f }; // cmps
	constexpr float 		Gold{ 0.f };
	constexpr float 		Armor{ 0.f };

}

namespace ItemConstants
{
	// 스테미나 소모량
	constexpr std::array<float, static_cast<size_t>(Common::ItemType::End)> Stamina{
		0.f,	// None
		0.f,    // Pistol
		1.f,    // Hammer
		1.f,    // Helmet
		1.f,    // Meat
		0.f,    // Whiskey
		0.f,    // EnhancePower
		0.f,    // Letter
		0.f,    // Wheel

	};

	constexpr float HelmetValue{ 3.f };
	constexpr float MeatValue{ 10.f };
	constexpr float WhiskeyValue{ 1.f };
	constexpr float WheelSpeed{ 1000.f }; // cmps
	constexpr float WheelTime{ 5.f }; // seconds

}

namespace MapConstants
{
	constexpr float BankX{ 0.f };
	constexpr float BankY{ 0.f };
	constexpr float BankWidth{ 1000.f };
	constexpr float BankHeight{ 1000.f };
}

namespace GameConstants
{
	constexpr float TurnTime{ 20.f }; // seconds
}

// --
// room
// --

constexpr uint32 MaxPlayerCount{ 5 };


COMMON_END