#include "HUDProgressBar.h"
#include <Render/Renderer.h>
#include <Render/Cell.h>

#include <algorithm>

namespace Craft
{
	HUDProgressBar::HUDProgressBar(const Vector2& position, const Vector2& size, float maxValue, const ColorRGB& fillColor, const ColorRGB& emptyColor)
		: HUDWidget(position, size), maxValue((std::max)(1.0f, maxValue)), fillColor(fillColor), emptyColor(emptyColor)
	{
		sortingOrder = 1020;
	}

	void HUDProgressBar::Draw()
	{
		if (!visible)
		{
			return;
		}

		if (size.x <= 0 || size.y <= 0)
		{
			return;
		}

		// 크기가 변경됐는지 확인
		if (barSprite.GetWidth() != size.x || barSprite.GetHeight() != size.y)
		{
			spriteDirty = true;
		}

		if (spriteDirty)
		{
			RebuildSprite();
		}

		Renderer::Get().SubmitScreenSpace(barSprite, position, sortingOrder);
	}

	void HUDProgressBar::SetValue(float newValue)
	{
		// 값은 0 ~ maxValue 범위로 제한
		const float clampedValue = std::clamp(newValue, 0.0f, maxValue);

		if (value == clampedValue)
		{
			return;
		}

		value = clampedValue;

		// 실제 표시 비율이 바뀌므로 Sprite 다시 생성
		spriteDirty = true;
	}

	void HUDProgressBar::SetMaxValue(float newMaxValue)
	{
		// 최대 값은 0보다 커야 함
		const float safeMaxValue = (std::max)(1.0f, newMaxValue);

		if (maxValue == safeMaxValue)
		{
			return;
		}

		maxValue = safeMaxValue;

		// 기존 value가 새로운 maxValue를 넘을 수 있으므로 제한
		value = std::clamp(value, 0.0f, maxValue);

		spriteDirty = true;
	}

	void HUDProgressBar::SetFillColor(const ColorRGB& newColor)
	{
		if (fillColor == newColor)
		{
			return;
		}

		fillColor = newColor;
		spriteDirty = true;
	}

	void HUDProgressBar::SetEmptyColor(const ColorRGB& newColor)
	{
		if (emptyColor == newColor)
		{
			return;
		}

		emptyColor = newColor;
		spriteDirty = true;
	}

	float HUDProgressBar::GetRatio() const
	{
		if (maxValue <= 0.0f)
		{
			return 0.0f;
		}

		return std::clamp(value / maxValue, 0.0f, 1.0f);
	}

	void HUDProgressBar::RebuildSprite()
	{
		barSprite = PixelSprite(size.x, size.y);

		// 현재 채워질 너비 계산
		// size.x = 20		ratio = 0.5
		// filledWidth = 10
		// ===================================
		const float ratio = GetRatio();

		const int filledWidth = static_cast<int>(static_cast<float>(size.x) * ratio);

		Cell fillPixel;
		fillPixel.character = ' ';
		fillPixel.foreground = fillColor;
		fillPixel.background = fillColor;

		Cell emptyPixel;
		emptyPixel.character = ' ';
		emptyPixel.foreground = emptyColor;
		emptyPixel.background = emptyColor;

		for (int y = 0; y < size.y; ++y)
		{
			for (int x = 0; x < size.x; ++x)
			{
				// 현재 x가 채워진 영역 안이면 fill
				// 아니면 empty
				const bool isFilled = x < filledWidth;

				barSprite.SetCell(x, y, isFilled ? fillPixel : emptyPixel);
			}
		}

		spriteDirty = false;
	}
}