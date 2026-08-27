#pragma once

#include <Core/Core.h>
#include <cstdint>

namespace Craft
{
	struct CRAFT_API ColorRGB
	{
		// 0~255의 고정된 rgb값 -> uint8_t
		uint8_t r = 255;
		uint8_t g = 255;
		uint8_t b = 255;

		ColorRGB() = default;

		ColorRGB(uint8_t r, uint8_t g, uint8_t b)
			: r(r), g(g), b(b)
		{ }

		// 비교연산자 오버로딩	-> 최적화: 비교해서 바뀐 값만 출력
		bool operator==(const ColorRGB& other) const
		{
			return (r == other.r) && (g == other.g) && (b == other.b);
		}

		bool operator!=(const ColorRGB& other) const
		{
			return !(*this == other);
		}
	};
}