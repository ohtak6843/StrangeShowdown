#pragma once
#include <cmath>
#include <limits>


struct Vec3f
{
	float x{ 0.f };
	float y{ 0.f };
	float z{ 0.f };

	constexpr Vec3f() = default;
	constexpr Vec3f(const Vec3f&) = default;
	constexpr Vec3f(const float x, const float y, const float z) :
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

	Vec3f& Normalize()
	{
		float length{ std::sqrt(x * x + y * y + z * z) };
		if (length > std::numeric_limits<float>::epsilon()) {
			x = x / length;
			y = y / length;
			z = z / length;
		}
		else {
			x = 0.f;
			y = 0.f;
			z = 0.f;
		}
		return *this;
	}

	float Dot(const Vec3f& rhs) const
	{
		return x * rhs.x + y * rhs.y + z * rhs.z;
	}
};
