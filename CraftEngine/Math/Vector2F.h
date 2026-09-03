#pragma once
#include <Core/Core.h>

namespace Craft
{
	class CRAFT_API Vector2F
	{
	public:
		Vector2F(float x = 0.0f, float y = 0.0f);
		~Vector2F() = default;

		// 연산자 오버로딩

		// 사칙 연산자 오버로딩
		Vector2F operator+(const Vector2F& other) const;
		Vector2F operator-(const Vector2F& other) const;

		Vector2F operator*(const Vector2F& other) const;
		Vector2F operator*(float scalar) const;

		Vector2F operator/(const Vector2F& other) const;
		Vector2F operator/(float scalar) const;

		// 증감 연산자 오버로딩
		Vector2F& operator+=(const Vector2F& other);
		Vector2F& operator-=(const Vector2F& other);

		// 대입 연산자 오버로딩
		Vector2F& operator=(const Vector2F& other);

		// 비교 연산자 오버로딩
		bool operator==(const Vector2F& other) const;
		bool operator!=(const Vector2F& other) const;

		// 자주 사용할 값을 전역 변수로 선언
		static Vector2F Zero;
		static Vector2F One;
		static Vector2F Right;
		static Vector2F Up;

	public:
		// 좌표계 값으로 사용하기 위한 변수
		float x = 0.0f;
		float y = 0.0f;
	};

}