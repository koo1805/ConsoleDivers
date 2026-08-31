#include "Vector2F.h"
#include <cassert>

namespace Craft
{
	Vector2F Vector2F::Zero(0.0f, 0.0f);
	Vector2F Vector2F::One(1.0f, 1.0f);
	Vector2F Vector2F::Right(1.0f, 0.0f);
	Vector2F Vector2F::Up(0.0f, -1.0f);

	Vector2F::Vector2F(float x, float y)
		: x(x), y(y)
	{ }

	Vector2F Vector2F::operator+(const Vector2F& other) const
	{
		return Vector2F(x + other.x, y + other.y);
	}

	Vector2F Vector2F::operator-(const Vector2F& other) const
	{
		return Vector2F(x - other.x, y - other.y);
	}

	Vector2F Vector2F::operator*(const Vector2F& other) const
	{
		return Vector2F(x * other.x, y * other.y);
	}

	Vector2F Vector2F::operator*(float scalar) const
	{
		return Vector2F(x * scalar, y * scalar);
	}

	Vector2F Vector2F::operator/(const Vector2F& other) const
	{
		// 어서트
		assert(other.x != 0 && other.y != 0);
		return Vector2F(x / other.x, y / other.y);
	}

	Vector2F Vector2F::operator/(float scalar) const
	{
		assert(scalar != 0);
		return Vector2F(x / scalar, y / scalar);
	}

	Vector2F& Vector2F::operator+=(const Vector2F& other)
	{
		x += other.x;
		y += other.y;

		return *this;
	}

	Vector2F& Vector2F::operator-=(const Vector2F& other)
	{
		x -= other.x;
		y -= other.y;

		return *this;
	}

	Vector2F& Vector2F::operator=(const Vector2F& other)
	{
		x = other.x;
		y = other.y;

		return *this;
	}

	bool Vector2F::operator==(const Vector2F& other) const
	{
		return (x == other.x) && (y == other.y);
	}

	bool Vector2F::operator!=(const Vector2F& other) const
	{
		//return (x != other.x) || (y != other.y);
		return !(*this == other);
	}
}