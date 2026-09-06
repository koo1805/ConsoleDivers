#pragma once

#include <HUD/HUDWidget.h>

#include <Math/ColorRGB.h>
#include <Render/Sprite/PixelSprite.h>

namespace Craft
{
	// HUD Progress Bar
	// 문자열을 사용하지 않고 PixelSprite Cell만 사용
	// ============================================================
	class CRAFT_API HUDProgressBar : public HUDWidget
	{
	public:
		HUDProgressBar() = default;

		HUDProgressBar(const Vector2& position, const Vector2& size, float maxValue, const ColorRGB& fillColor,const ColorRGB& emptyColor);

	public:
		virtual void Draw() override;

	public:
		// 현재 값 설정
		void SetValue(float newValue);

		inline float GetValue() const { return value; }

		// 최대 값 설정
		void SetMaxValue(float newMaxValue);

		inline float GetMaxValue() const { return maxValue; }

		// 채워진 부분 색상
		void SetFillColor(const ColorRGB& newColor);

		// 비어있는 부분 색상
		void SetEmptyColor(const ColorRGB& newColor);

		// 현재 비율 반환
		float GetRatio() const;

	private:
		// 현재 값/크기/색상에 맞춰 Sprite 다시 생성
		void RebuildSprite();

	private:
		float value = 0.0f;

		// 0으로 나누는 것을 방지하기 위해 기본값 1
		float maxValue = 1.0f;

		ColorRGB fillColor = ColorRGB(255, 255, 255);
		ColorRGB emptyColor = ColorRGB(50, 50, 50);

		PixelSprite barSprite;

		// 값/색상/크기가 변경된 경우에만 재생성
		bool spriteDirty = true;
	};
}