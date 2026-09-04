#include "EnemyVisualBase.h"

void EnemyVisualBase::SetFacingRight(bool facingRight)
{
	// CharacterPart와 연결이 안되었다면 건너뛰기
	if (!isInitialized)
	{
		return;
	}

	// 이미 같은 방향이면
	if (isfacingRight == facingRight)
	{
		return;
	}

	this->isfacingRight = facingRight;

	// Sprite 반영
	ApplyFacing();
}

void EnemyVisualBase::SetInitialized(bool initialized)
{
	isInitialized = initialized;
}

Craft::PixelSprite EnemyVisualBase::FlipHorizontal(const Craft::PixelSprite& sprite) const
{
	const int width = sprite.GetWidth();
	const int height = sprite.GetHeight();

	Craft::PixelSprite flippedSprite(width, height);

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			const int sourceX = width - 1 - x;

			flippedSprite.SetCell(sourceX, y, sprite.GetCell(x, y));
		}
	}

	return flippedSprite;
}
