#pragma once

#include <Math/ColorRGB.h>

namespace Craft
{
	// 콘솔의 cell단위 렌더링
	struct Cell
	{
		char character = ' ';
		ColorRGB foreground = ColorRGB(255, 255, 255);
		ColorRGB background = ColorRGB(0, 0, 0);

		// Cell비교
		bool operator==(const Cell& other) const
		{
			return character == other.character && foreground == other.foreground && background == other.background;
		}

		bool operator!=(const Cell& other) const
		{
			return !(*this == other);
		}
	};
}