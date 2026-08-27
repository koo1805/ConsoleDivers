#include "PixelSprite.h"

namespace Craft
{
	PixelSprite::PixelSprite(int width, int height)
		: width(width),
		height(height),
		cells(width * height)
	{
		// 생성된 Cell은 기본적으로 투명하게 설정
		for (Cell& cell : cells)
		{
			cell.character = '\0';
		}
	}

	void PixelSprite::SetCell(int x, int y, const Cell& cell)
	{
		// Sprite영역을 벗어나면 건너뛰기
		if (x < 0 || x >= width || y < 0 || y >= height)
		{
			return;
		}

		const int index = y * width + x;

		cells[index] = cell;
	}

	const Cell& PixelSprite::GetCell(int x, int y) const
	{
		const int index = y * width + x;

		return cells[index];
	}
}