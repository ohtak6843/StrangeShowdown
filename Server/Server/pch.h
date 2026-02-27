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

#define UENUM(type)
#define UMETA(type)
#define ENUM_CLASS_FLAGS(type)

#include "struct.h"
#include "type.h"
#include "define.h"

using RecvBuffer = std::array<int8, BUFFER_SIZE>;
using SendBuffer = std::vector<int8>;

using SessionPtr = std::shared_ptr<class Session>;
using PlayerPtr = std::shared_ptr<class Player>;


