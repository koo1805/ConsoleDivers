#pragma once

#include <HUD/HUDWidget.h>
#include <Math/ColorRGB.h>
#include <Render/Sprite/PixelSprite.h>

namespace Craft
{
	// HUD 사각형 테두리
	// 내부는 투명하게 두고 외곽 Cell만 렌더링
	// ============================================================
	class CRAFT_API HUDBorder : public HUDWidget
	{
	public:
		HUDBorder() = default;

		HUDBorder(const Vector2& position, const Vector2& size, const ColorRGB& borderColor);

	public:
		virtual void Draw() override;

		void SetBorderColor(const ColorRGB& newColor);

		inline const ColorRGB& GetBorderColor() const { return borderColor; }

	private:
		void RebuildSprite();

	private:
		ColorRGB borderColor = ColorRGB(255, 255, 255);

		PixelSprite borderSprite;

		bool spriteDirty = true;
	};
}