#pragma once
#define NOMINMAX
#include <WS2tcpip.h>
#include <MSWSock.h>

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")

#include <thread>
#include <vector>
#include <array>
#include <print>
#include <iostream>
#include <random>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <queue>
#include <concurrent_unordered_set.h>
#include <concurrent_unordered_map.h>
#include <concurrent_queue.h>
#include <chrono>
#include <atomic>
#include <mutex>
#include <numeric>
#include <functional>

#include "struct.h"
#include "type.h"
#include "protocol.h"
#include "define.h"

// using PacketType = std::array<char, BUFFER_SIZE>;

