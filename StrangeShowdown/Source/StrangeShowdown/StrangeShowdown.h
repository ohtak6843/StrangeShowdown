// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

struct Vec3f
{
	float x{ 0.f };
	float y{ 0.f };
	float z{ 0.f };

	Vec3f() = default;
	Vec3f(const Vec3f&) = default;
	Vec3f(const float x, const float y, const float z) :
		x{ x },
		y{ y },
		z{ z }
	{
	}

	Vec3f operator+(const Vec3f& rhs) const
	{
		return Vec3f{ x + rhs.x, y + rhs.y, z + rhs.z };
	}

	Vec3f operator-(const Vec3f& rhs) const
	{
		return Vec3f{ x - rhs.x, y - rhs.y, z - rhs.z };
	}

	Vec3f operator-()
	{
		return Vec3f{ -x, -y, -z };
	}

	Vec3f operator*(const float val)
	{
		return Vec3f{ x * val, y * val, z * val };
	}

	float Dot(const Vec3f& rhs) const
	{
		return x * rhs.x + y * rhs.y + z * rhs.z;
	}
};

#include "../../Server/Server/protocol.h"