#pragma once

#include <Actor/Projectile/ProjectileBase.h>

class PelletProjectile : public ProjectileBase
{
public:
    PelletProjectile(
        const Craft::Vector2F& position,
        const Craft::Vector2F& direction,
        float speed,
        float lifeTime,
        bool isHoming = false
    );

    virtual ~PelletProjectile() override = default;

private:
    Craft::PixelSprite CreatePelletSprite() const;
};