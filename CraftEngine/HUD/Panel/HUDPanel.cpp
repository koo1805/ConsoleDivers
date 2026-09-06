#include "HUDPanel.h"
#include <Render/Renderer.h>
#include <Render/Cell.h>

namespace Craft
{
	HUDPanel::HUDPanel(const Vector2& position, const Vector2& size, const ColorRGB& backgroundColor)
		: HUDWidget(position, size), backgroundColor(backgroundColor)
	{}

	void HUDPanel::Draw()
	{
		// 숨겨진 HUD는 렌더링하지 않음
		if (!visible)
		{
			return;
		}

		// 잘못된 크기는 그리지 않음
		if (size.x <= 0 || size.y <= 0)
		{
			return;
		}

		// SetSize()를 통해 크기가 바뀌었는지 검사
		// HUDPanel은 PixelSprite를 내부적으로 캐싱하므로 매 프레임 Sprite를 새로 만들 필요가 없음
		// ========================================================================================
		if (panelSprite.GetWidth() != size.x || panelSprite.GetHeight() != size.y)
		{
			spriteDirty = true;
		}

		if (spriteDirty)
		{
			RebuildSprite();
		}

		// HUD이므로 ScreenSpace 렌더링
		// Camera 위치와 관계없이 항상 같은 화면 위치에 나타남
		// =======================================================
		Renderer::Get().SubmitScreenSpace(panelSprite, position, sortingOrder);
	}

	void HUDPanel::SetBackgroundColor(const ColorRGB& newColor)
	{
		if (backgroundColor == newColor)
		{
			return;
		}

		backgroundColor = newColor;

		// 색상이 변했으므로 PixelSprite를 다음 Draw에서 재생성
		spriteDirty = true;
	}

	void HUDPanel::RebuildSprite()
	{
		panelSprite = PixelSprite(size.x, size.y);

		// Console PixelSprite에서 character == '\0' 은 투명 Cell
		// 따라서 Panel은 ' ' 문자를 넣어서 실제 Cell이 존재하도록 만들어야 함
		// ====================================================================
		Cell pixel;
		pixel.character = ' ';

		// 공백 문자이므로 Foreground는 사실 중요하지 않지만 동일한 색으로 맞춰 둠
		pixel.foreground = backgroundColor;
		pixel.background = backgroundColor;

		for (int y = 0; y < size.y; ++y)
		{
			for (int x = 0; x < size.x; ++x)
			{
				panelSprite.SetCell(x, y, pixel);
			}
		}

		spriteDirty = false;
	}
}