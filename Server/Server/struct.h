#pragma once

struct Vec2f
{
	float x{ 0.f };
	float y{ 0.f };

	Vec2f() = default;
	Vec2f(const Vec2f&) = default;
	Vec2f(const float x, const float y) :
		x{ x },
		y{ y }
	{
	}

	Vec2f operator+(const Vec2f& rhs) const
	{
		return Vec2f{ x + rhs.x, y + rhs.y };
	}

	Vec2f operator-(const Vec2f& rhs) const
	{
		return Vec2f{ x - rhs.x, y - rhs.y };
	}

	Vec2f operator-()
	{
		return Vec2f{ -x, -y };
	}

	Vec2f operator*(const float val)
	{
		return Vec2f{ x * val, y * val };
	}

	Vec2f& Normalize()
	{
		float length{ std::sqrt(x * x + y * y) };
		if (length > std::numeric_limits<float>::epsilon()) {
			x = x / length;
			y = y / length;
		}
		else {
			x = 0.f;
			y = 0.f;
		}
		return *this;
	}

	float Dot(const Vec2f& rhs) const
	{
		return x * rhs.x + y * rhs.y;
	}
};

enum class IOOperation
{
	NONE,
	ACCEPT,
	RECV,
	SEND
};

enum class IOState
{
	NONE,
	CONNECT,
	INGAME,
	DISCONNECT
};
