#include "HUDNumber.h"
#include <Render/Renderer.h>

#include <algorithm>

namespace Craft
{
	HUDNumber::HUDNumber(const Vector2& position)
		: HUDWidget(position)
	{
		sortingOrder = 1020;
	}

	void HUDNumber::Draw()
	{
		if (!visible)
		{
			return;
		}

		if (!HasValidDigitSprites())
		{
			return;
		}

		// 실제 필요한 자릿수와 최소 자릿수 중 큰 값을 사용
		const int naturalDigitCount = CalculateNaturalDigitCount();

		const int totalDigitCount = (std::max)(naturalDigitCount, minDigits);

		// 가장 왼쪽 자리 계산에 사용할 10의 제곱수
		// 세 자리라면: 100 -> 10 -> 1
		int divisor = 1;

		for (int i = 1; i < totalDigitCount; ++i)
		{
			divisor *= 10;
		}

		int drawX = position.x;

		for (int digitIndex = 0; digitIndex < totalDigitCount; ++digitIndex)
		{
			// 현재 자리의 숫자 추출
			// 198 / 100 % 10 -> 1
			// 198 / 10  % 10 -> 9
			// 198 / 1   % 10 -> 8
			// // 정상적으로는 항상 0 ~ 9 범위가 나옴
			// ===============================================================
			const int digit = (value / divisor) % 10;

			if (digit < 0 || digit >= static_cast<int>(digitSprites.size()))
			{
				return;
			}

			const PixelSprite* digitSprite = digitSprites[static_cast<std::size_t>(digit)];

			if (digitSprite != nullptr)
			{
				Renderer::Get().SubmitScreenSpace(*digitSprite, Vector2(drawX, position.y), sortingOrder);

				// 현재 숫자 Sprite 너비만큼 다음 위치 이동
				drawX += digitSprite->GetWidth();

				// 숫자 사이 간격 추가
				drawX += digitSpacing;
			}

			// 다음 자리
			if (divisor > 1)
			{
				divisor /= 10;
			}
		}

		// 마지막 숫자 뒤의 간격은 실제 HUD 너비에서 제외
		if (totalDigitCount > 0)
		{
			drawX -= digitSpacing;
		}

		// 현재 출력 결과의 전체 크기 저장
		size.x = drawX - position.x;

		// 숫자 Sprite들의 높이가 동일하다는 전제
		if (digitSprites[0] != nullptr)
		{
			size.y = digitSprites[0]->GetHeight();
		}
	}

	void HUDNumber::SetValue(int newValue)
	{
		// 현재 HUD에서 음수 표현은 필요하지 않으므로 음수는 0으로 제한
		value = (std::max)(0, newValue);
	}

	void HUDNumber::SetDigitSprites(const std::array<const PixelSprite*, 10>& newDigitSprites)
	{
		digitSprites = newDigitSprites;
	}

	void HUDNumber::SetDigitSpacing(int newSpacing)
	{
		// 음수 간격은 우선 지원하지 않음
		digitSpacing = (std::max)(0, newSpacing);
	}

	void HUDNumber::SetMinDigits(int newMinDigits)
	{
		// int 범위에서 지나치게 긴 숫자 출력을 방지
		// 현재 HUD 용도로는 10자리면 충분
		minDigits = std::clamp(newMinDigits, 1, 10);
	}

	int HUDNumber::CalculateNaturalDigitCount() const
	{
		if (value == 0)
		{
			return 1;
		}

		int count = 0;
		int tempValue = value;

		while (tempValue > 0)
		{
			tempValue /= 10;
			++count;
		}

		return count;
	}

	bool HUDNumber::HasValidDigitSprites() const
	{
		for (const PixelSprite* digitSprite : digitSprites)
		{
			if (digitSprite == nullptr)
			{
				return false;
			}
		}

		return true;
	}
}