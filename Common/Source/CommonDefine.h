#pragma once

#include "CommonNamespace.h"
#include "CommonEnum.h"
#include <string_view>

COMMON_START

// --
// network constants
// --
constexpr int PORT_NUMBER{ 7777 };
constexpr const char* SERVER_IP{ "127.0.0.1" };

constexpr float SendMoveTime{ 0.08f }; 
constexpr float MAX_NETWORK_DELAY_MS{ 100.f }; // 최대 네트워크 딜레이


// --
// player constants
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
	constexpr float 		Atk{ 10.f };
	constexpr float 		Def{ 0.f };
	constexpr float 		Speed{ 5.f };

}

COMMON_END