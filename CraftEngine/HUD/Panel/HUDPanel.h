#pragma once

#include <HUD/HUDWidget.h>
#include <Math/ColorRGB.h>
#include <Render/Sprite/PixelSprite.h>

namespace Craft
{
	// 단색 사각형 HUD 영역
	// 여러 HUD의 기본 배경으로 사용
	// ============================================================
	class CRAFT_API HUDPanel : public HUDWidget
	{
	public:
		HUDPanel() = default;

		HUDPanel(const Vector2& position, const Vector2& size, const ColorRGB& backgroundColor);

	public:
		virtual void Draw() override;

		// Panel 색 변경
		void SetBackgroundColor(const ColorRGB& newColor);

		inline const ColorRGB& GetBackgroundColor() const { return backgroundColor; }

	private:
		// 현재 size/color를 이용해 PixelSprite를 다시 생성
		void RebuildSprite();

	private:
		ColorRGB backgroundColor = ColorRGB(0, 0, 0);

		// Panel도 Renderer 입장에서는 하나의 PixelSprite
		PixelSprite panelSprite;

		// 색이 변경되었을 때 Sprite 재생성이 필요한지
		bool spriteDirty = true;
	};
}