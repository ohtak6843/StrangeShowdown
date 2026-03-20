#pragma once

#include "CommonNamespace.h"

COMMON_START

constexpr int PORT_NUMBER{ 7777 };
constexpr const char* SERVER_IP{ "127.0.0.1" };

constexpr float MOVE_PACKET_TIME_MS{ 75.f }; // 초당 13.3회
constexpr float MAX_NETWORK_DELAY_MS{ 100.f }; // 최대 네트워크 딜레이

COMMON_END