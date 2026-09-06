#include "NumberSprite.h"

#include <Render/Cell.h>
#include <Math/ColorRGB.h>

namespace
{
	constexpr int DigitWidth = 3;
	constexpr int DigitHeight = 5;

	// ------------------------------------------------------------
	// 3 x 5 Pixel 숫자 패턴
	//
	// 1 = 표시 Cell
	// 0 = 투명 Cell
	// ------------------------------------------------------------
	const char* DigitPatterns[10][DigitHeight]
	{
		// 0
		{
			"111",
			"101",
			"101",
			"101",
			"111"
		},

		// 1
		{
			"010",
			"110",
			"010",
			"010",
			"111"
		},

		// 2
		{
			"111",
			"001",
			"111",
			"100",
			"111"
		},

		// 3
		{
			"111",
			"001",
			"111",
			"001",
			"111"
		},

		// 4
		{
			"101",
			"101",
			"111",
			"001",
			"001"
		},

		// 5
		{
			"111",
			"100",
			"111",
			"001",
			"111"
		},

		// 6
		{
			"111",
			"100",
			"111",
			"101",
			"111"
		},

		// 7
		{
			"111",
			"001",
			"001",
			"001",
			"001"
		},

		// 8
		{
			"111",
			"101",
			"111",
			"101",
			"111"
		},

		// 9
		{
			"111",
			"101",
			"111",
			"001",
			"111"
		}
	};

	Craft::Cell MakeDigitPixel()
	{
		const Craft::ColorRGB color(230, 230, 220);

		Craft::Cell cell;

		cell.character = ' ';
		cell.foreground = color;
		cell.background = color;

		return cell;
	}
}

NumberSprite::NumberSprite()
{
	for (int digit = 0; digit < 10; ++digit)
	{
		digitSprites[static_cast<std::size_t>(digit)] = CreateDigitSprite(digit);
	}

	colonSprite = CreateColonSprite();
	infinitySprite = CreateInfinitySprite();
}

Craft::PixelSprite NumberSprite::CreateDigitSprite(int digit) const
{
	Craft::PixelSprite sprite(DigitWidth, DigitHeight);

	if (digit < 0 || digit >= 10)
	{
		return sprite;
	}

	const Craft::Cell pixel = MakeDigitPixel();

	for (int y = 0; y < DigitHeight; ++y)
	{
		for (int x = 0; x < DigitWidth; ++x)
		{
			if (DigitPatterns[digit][y][x] != '1')
			{
				continue;
			}

			sprite.SetCell(x, y, pixel);
		}
	}

	return sprite;
}

Craft::PixelSprite
NumberSprite::CreateColonSprite() const
{
	// 숫자 높이와 동일하게 1 x 5
	Craft::PixelSprite sprite(1, DigitHeight);

	const Craft::Cell pixel = MakeDigitPixel();

	// :
	//
	// .
	// #
	// .
	// #
	// .
	sprite.SetCell(0, 1, pixel);

	sprite.SetCell(0, 3, pixel);

	return sprite;
}

Craft::PixelSprite NumberSprite::CreateInfinitySprite() const
{
	constexpr int width = 9;
	constexpr int height = 5;

	Craft::PixelSprite sprite(width, height);

	const Craft::Cell pixel = MakeDigitPixel();

	const char* pattern[height]
	{
		"000000000",
		"011101110",
		"110111011",
		"011101110",
		"000000000"
	};

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			if (pattern[y][x] != '1')
			{
				continue;
			}

			sprite.SetCell(x, y, pixel);
		}
	}

	return sprite;
}

std::array<const Craft::PixelSprite*, 10> NumberSprite::GetDigitPointers() const
{
	std::array<const Craft::PixelSprite*, 10> result{};

	for (std::size_t index = 0; index < digitSprites.size(); ++index)
	{
		result[index] = &digitSprites[index];
	}

	return result;
}