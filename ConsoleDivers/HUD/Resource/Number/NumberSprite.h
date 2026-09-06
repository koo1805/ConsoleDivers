#pragma once

#include <Render/Sprite/PixelSprite.h>

#include <array>

class NumberSprite
{
public:
	NumberSprite();

public:
	// HUDNumber에 넘길 0 ~ 9 Sprite 포인터 배열
	std::array<const Craft::PixelSprite*, 10> GetDigitPointers() const;

	// 타이머에서 사용할 ':' Sprite
	inline const Craft::PixelSprite* GetColonSprite() const { return &colonSprite; }

	// 무한 Sprite
	inline const Craft::PixelSprite* GetInfinitySprite() const { return &infinitySprite; }

private:
	// 숫자 한 개 생성
	Craft::PixelSprite CreateDigitSprite(int digit) const;

	// ':' 생성
	Craft::PixelSprite CreateColonSprite() const;

	// 무한 생성
	Craft::PixelSprite CreateInfinitySprite() const;

private:
	std::array<Craft::PixelSprite, 10> digitSprites;

	Craft::PixelSprite colonSprite;

	Craft::PixelSprite infinitySprite;
};