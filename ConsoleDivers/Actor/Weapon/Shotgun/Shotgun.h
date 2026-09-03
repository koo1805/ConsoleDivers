#pragma once

#include <Actor/Weapon/WeaponBase.h>

class Shotgun : public WeaponBase
{
	TYPE_DECLARATIONS(Shotgun, WeaponBase)

public:
	Shotgun(const Craft::Vector2F& position = Craft::Vector2F::Zero);

	virtual ~Shotgun() override = default;

public:
	virtual void Fire() override;

private:
	// Sprite
	Craft::PixelSprite CreateShotgunSprite() const;

};

