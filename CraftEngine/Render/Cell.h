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
	};
}