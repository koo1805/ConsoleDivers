#include "NormalEnemyVisual.h"
#include <Render/Cell.h>
#include <Math/ColorRGB.h>
#include <Actor/Character/CharacterPart.h>

namespace
{
	// PixelSprite에서 사용할
	// 하나의 색상 Cell 생성
	Craft::Cell MakePixel(
		const Craft::ColorRGB& color)
	{
		Craft::Cell cell;

		// 현재 PixelSprite 방식은
		// 문자 자체보다 배경색을 Pixel처럼 사용
		cell.character = ' ';

		cell.foreground = color;
		cell.background = color;

		return cell;
	}
}

void NormalEnemyVisual::Initialize(CharacterPart* body)
{
	// 부위 연결
	this->body = body;

	if (!this->body)
	{
		return;
	}

	GenerateVisualData();

	// Visual 초기화
	SetInitialized(true);

	// 기본 방향 적용
	ApplyFacing();
}

void NormalEnemyVisual::SetIdleFrame(int frameIndex)
{
	if (!IsInitialized() || !body)
	{
		return;
	}

	bodySprite = idleSprite;

	// 바라보는 방향까지 반영
	ApplyFacing();
}

void NormalEnemyVisual::SetWalkFrame(int frameIndex)
{
	if (!IsInitialized() || !body)
	{
		return;
	}

	if (frameIndex < 0 || frameIndex >= static_cast<int>(walkSprites.size()))
	{
		return;
	}

	bodySprite = walkSprites[frameIndex];

	// 바라보는 방향까지 반영
	ApplyFacing();
}

void NormalEnemyVisual::SetAttackFrame(int frameIndex)
{
	if (!IsInitialized() || !body)
	{
		return;
	}

	if (frameIndex < 0 || frameIndex >= static_cast<int>(attackSprites.size()))
	{
		return;
	}

	bodySprite = attackSprites[frameIndex];

	// 바라보는 방향까지 반영
	ApplyFacing();
}

void NormalEnemyVisual::SetDeathFrame(int frameIndex)
{
	if (!IsInitialized() || !body)
	{
		return;
	}

	bodySprite = deathSprite;

	// 바라보는 방향까지 반영
	ApplyFacing();
}

void NormalEnemyVisual::ApplyFacing()
{
	if (!IsInitialized() || !body)
	{
		return;
	}

	// 오른쪽
	if (IsFacingRight())
	{
		body->SetSprite(bodySprite);

		return;
	}

	// 왼쪽
	body->SetSprite(FlipHorizontal(bodySprite));
}

void NormalEnemyVisual::GenerateVisualData()
{
	idleSprite = CreateIdleSprite();

	// 기본 Sprite도 Idle로 시작
	bodySprite = idleSprite;

	// Walk
	for (int index = 0; index < static_cast<int>(walkSprites.size()); ++index)
	{
		walkSprites[index] = CreateWalkSprite(index);
	}
	
	// Attack
	for (int index = 0; index < static_cast<int>(attackSprites.size()); ++index)
	{
		attackSprites[index] = CreateAttackSprite(index);
	}

	// Death
	deathSprite = CreateDeathSprite();
}

Craft::PixelSprite NormalEnemyVisual::CreateIdleSprite() const
{
	constexpr int Width = 7;
	constexpr int Height = 7;


	// 테스트용 Enemy 모양
	//
	// O : 외곽
	// B : 몸통
	// E : 눈
	// . : 투명
	//
	const char* pixels[Height] =
	{
		"..OOO..",
		".OBBBO.",
		"OBBEBEO",
		"OBBBBO.",
		".OBBBO.",
		"O.O.O.O",
		"O.O.O.O"
	};


	const Craft::Cell outlinePixel =
		MakePixel(Craft::ColorRGB(40, 30, 30));

	const Craft::Cell bodyPixel =
		MakePixel(Craft::ColorRGB(150, 60, 45));

	const Craft::Cell eyePixel =
		MakePixel(Craft::ColorRGB(255, 220, 80));


	Craft::PixelSprite sprite(
		Width,
		Height);


	for (int y = 0; y < Height; ++y)
	{
		for (int x = 0; x < Width; ++x)
		{
			switch (pixels[y][x])
			{
			case 'O':

				sprite.SetCell(
					x,
					y,
					outlinePixel);

				break;


			case 'B':

				sprite.SetCell(
					x,
					y,
					bodyPixel);

				break;


			case 'E':

				sprite.SetCell(
					x,
					y,
					eyePixel);

				break;


			case '.':
			default:

				// PixelSprite 기본값이 투명이므로
				// 아무것도 설정하지 않는다.
				break;
			}
		}
	}


	return sprite;
}

Craft::PixelSprite
NormalEnemyVisual::CreateWalkSprite(int frameIndex) const
{
	constexpr int Width = 7;
	constexpr int Height = 7;

	const char* frame0[Height] =
	{
		"..OOO..",
		".OBBBO.",
		"OBBEBEO",
		"OBBBBO.",
		".OBBBO.",
		".O..O..",
		"O....O."
	};


	const char* frame1[Height] =
	{
		"..OOO..",
		".OBBBO.",
		"OBBEBEO",
		"OBBBBO.",
		".OBBBO.",
		"..O..O.",
		".O....O"
	};


	const char** pixels = frameIndex == 0 ? frame0 : frame1;

	const Craft::Cell outlinePixel = MakePixel(Craft::ColorRGB(40, 30, 30));

	const Craft::Cell bodyPixel = MakePixel(Craft::ColorRGB(150, 60, 45));

	const Craft::Cell eyePixel = MakePixel(Craft::ColorRGB(255, 220, 80));

	Craft::PixelSprite sprite(Width, Height);

	for (int y = 0; y < Height; ++y)
	{
		for (int x = 0; x < Width; ++x)
		{
			switch (pixels[y][x])
			{
			case 'O':
				sprite.SetCell(x, y, outlinePixel);
				break;

			case 'B':
				sprite.SetCell(x, y, bodyPixel);
				break;

			case 'E':
				sprite.SetCell(x, y, eyePixel);
				break;

			default:
				break;
			}
		}
	}

	return sprite;
}

Craft::PixelSprite
NormalEnemyVisual::CreateAttackSprite(int frameIndex) const
{
	constexpr int Width = 7;
	constexpr int Height = 7;

	const char* frame0[Height] =
	{
		"..OOO..",
		".OBBBO.",
		"OBBEBEO",
		"OBBBBO.",
		".OBBBO.",
		"O.O.O.O",
		"O.O.O.O"
	};


	const char* frame1[Height] =
	{
		"...OOO.",
		"..OBBBO",
		".OBBEBE",
		"..OBBBO",
		"...OBBB",
		"..O.O..",
		".O...O."
	};


	const char** pixels = frameIndex == 0 ? frame0 : frame1;


	const Craft::Cell outlinePixel = MakePixel(Craft::ColorRGB(40, 30, 30));

	const Craft::Cell bodyPixel = MakePixel(Craft::ColorRGB(150, 60, 45));

	const Craft::Cell eyePixel = MakePixel(Craft::ColorRGB(255, 220, 80));

	Craft::PixelSprite sprite(Width, Height);


	for (int y = 0; y < Height; ++y)
	{
		for (int x = 0; x < Width; ++x)
		{
			switch (pixels[y][x])
			{
			case 'O':
				sprite.SetCell(x, y, outlinePixel);
				break;

			case 'B':
				sprite.SetCell(x, y, bodyPixel);
				break;

			case 'E':
				sprite.SetCell(x, y, eyePixel);
				break;

			default:
				break;
			}
		}
	}

	return sprite;
}

Craft::PixelSprite NormalEnemyVisual::CreateDeathSprite() const
{
	constexpr int Width = 7;
	constexpr int Height = 7;

	// =========================================
	// Death Sprite
	// =========================================
	//
	// 형태는 Idle과 동일하게 유지하고
	// 전체 색상만 어둡게 만든다.
	//
	const char* pixels[Height] =
	{
		"..OOO..",
		".OBBBO.",
		"OBBEBEO",
		"OBBBBO.",
		".OBBBO.",
		"O.O.O.O",
		"O.O.O.O"
	};


	// 기존 색상보다 전체적으로 어둡게
	const Craft::Cell outlinePixel = MakePixel(Craft::ColorRGB(25, 19, 19));

	const Craft::Cell bodyPixel = MakePixel(Craft::ColorRGB(95, 38, 29));

	const Craft::Cell eyePixel = MakePixel(Craft::ColorRGB(160, 138, 50));

	Craft::PixelSprite sprite(Width, Height);

	for (int y = 0; y < Height; ++y)
	{
		for (int x = 0; x < Width; ++x)
		{
			switch (pixels[y][x])
			{
			case 'O':
				sprite.SetCell(x, y, outlinePixel);
				break;

			case 'B':
				sprite.SetCell(x, y, bodyPixel);
				break;

			case 'E':
				sprite.SetCell(x, y, eyePixel);
				break;

			case '.':
			default:

				break;
			}
		}
	}

	return sprite;
}
