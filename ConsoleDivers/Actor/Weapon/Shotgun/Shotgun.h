#pragma once

#include <Actor/Weapon/WeaponBase.h>

class Shotgun : public WeaponBase
{
	TYPE_DECLARATIONS(Shotgun, WeaponBase)

public:
	Shotgun(const Craft::Vector2F& position = Craft::Vector2F::Zero);

	virtual ~Shotgun() override = default;

public:
	virtual void StartFire(const Craft::Vector2F& aimDirection) override;

protected:
	// 방향 바뀔 때 로직
	virtual void OnFacingChanged() override;

private:
	// 오른쪽 Sprite
	Craft::PixelSprite CreateShotgunSprite() const;

	// 좌우 반전
	Craft::PixelSprite FlipHorizontal(const Craft::PixelSprite& sprite) const;

private:
	// 한 번만 만들어 저장
	Craft::PixelSprite rightPixel;
	Craft::PixelSprite leftPixel;

	// 한 번에 발사할 탄 수
	static constexpr int pelletCount = 5;

	// 탄 이동 속도
	static constexpr float projectileSpeed = 60.0f;

	// 탄 수명
	static constexpr float projectileLifeTime = 1.5f;

	// 탄 퍼짐 정도
	static constexpr float spreadAngle = 0.08f;

};

