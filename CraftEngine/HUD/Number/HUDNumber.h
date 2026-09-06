#pragma once

#include <HUD/HUDWidget.h>
#include <Render/Sprite/PixelSprite.h>

#include <array>

namespace Craft
{
	// 숫자 0~9 PixelSprite를 조합해 정수 값을 출력하는 HUD
	// 문자열 출력 아님
	// ============================================================
	class CRAFT_API HUDNumber : public HUDWidget
	{
	public:
		HUDNumber() = default;

		explicit HUDNumber(const Vector2& position);

	public:
		virtual void Draw() override;

		// 표시할 숫자
		void SetValue(int newValue);

		inline int GetValue() const { return value; }

		// 0 ~ 9에 대응되는 PixelSprite를 한 번에 설정
		// 배열 인덱스 자체가 숫자를 의미
		// [0] -> 숫자 0
		// [1] -> 숫자 1
		// ...
		// [9] -> 숫자 9
		// --------------------------------------------------------
		void SetDigitSprites(const std::array<const PixelSprite*, 10>& newDigitSprites);

		// 숫자 Sprite 사이의 간격
		void SetDigitSpacing(int newSpacing);

		// 최소 출력 자릿수
		void SetMinDigits(int newMinDigits);

		inline int GetMinDigits() const { return minDigits; }

	private:
		// 현재 숫자의 실제 자릿수
		int CalculateNaturalDigitCount() const;

		// 모든 0~9 Sprite가 설정되었는지 검사
		bool HasValidDigitSprites() const;

	private:
		int value = 0;

		// 최소 한 자리 출력
		int minDigits = 1;

		// 각 숫자 Sprite 사이의 공간
		int digitSpacing = 0;

		// 0~9 Sprite
		std::array<const PixelSprite*, 10> digitSprites =
		{
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr
		};
	};
}