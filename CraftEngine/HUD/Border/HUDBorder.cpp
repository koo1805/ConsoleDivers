#include "HUDBorder.h"
#include <Render/Renderer.h>
#include <Render/Cell.h>

namespace Craft
{
	HUDBorder::HUDBorder(const Vector2& position, const Vector2& size, const ColorRGB& borderColor)
		: HUDWidget(position, size), borderColor(borderColor)
	{
		// Panel보다 위에서 보이도록 기본값을 조금 높임
		sortingOrder = 1010;
	}

	void HUDBorder::Draw()
	{
		if (!visible)
		{
			return;
		}

		// 사각형 테두리를 만들려면 최소 2 x 2 필요
		if (size.x < 2 || size.y < 2)
		{
			return;
		}

		if (borderSprite.GetWidth() != size.x || borderSprite.GetHeight() != size.y)
		{
			spriteDirty = true;
		}

		if (spriteDirty)
		{
			RebuildSprite();
		}

		Renderer::Get().SubmitScreenSpace(borderSprite, position, sortingOrder);
	}

	void HUDBorder::SetBorderColor(const ColorRGB& newColor)
	{
		if (borderColor == newColor)
		{
			return;
		}

		borderColor = newColor;
		spriteDirty = true;
	}

	void HUDBorder::RebuildSprite()
	{
		// 새 PixelSprite의 Cell들은 PixelSprite 생성자에서 '\0'으로 초기화
		// 따라서 내부는 건드리지 않으면 자동으로 투명 영역
		// --------------------------------------------------------
		borderSprite = PixelSprite(size.x, size.y);

		Cell borderPixel;
		borderPixel.character = ' ';
		borderPixel.foreground = borderColor;
		borderPixel.background = borderColor;

		// 위 / 아래
		for (int x = 0; x < size.x; ++x)
		{
			borderSprite.SetCell(x, 0, borderPixel);

			borderSprite.SetCell(x, size.y - 1, borderPixel);
		}

		// 왼쪽 / 오른쪽
		for (int y = 0; y < size.y; ++y)
		{
			borderSprite.SetCell(0, y, borderPixel);

			borderSprite.SetCell(size.x - 1, y, borderPixel);
		}

		spriteDirty = false;
	}
}