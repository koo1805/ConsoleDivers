#include "Sprite.h"

namespace Craft
{
	Sprite::Sprite(std::initializer_list<std::string> lines)
		: lines(lines)
	{ }

	Sprite::Sprite(const std::vector<std::string>& lines)
		: lines(lines)
	{ }

	int Sprite::GetWidth() const
	{
		int width = 0;

		// 가장 긴 문자열의 길이를 Sprite의 너비로 사용
		for (const std::string& line : lines)
		{
			const int lineWidth = static_cast<int>(line.size());

			if (lineWidth > width)
			{
				width = lineWidth;
			}
		}
		return width;
	}

	int Sprite::GetHeight() const
	{
		// 문자열의 줄 개수를 높이로 사용
		return static_cast<int>(lines.size());
	}
}