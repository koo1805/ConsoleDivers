#include "PelletProjectile.h"
#include <Render/Cell.h>
#include <Math/ColorRGB.h>

namespace
{
    Craft::Cell MakePixel(
        const Craft::ColorRGB& color)
    {
        Craft::Cell cell;

        cell.character = ' ';
        cell.foreground = color;
        cell.background = color;

        return cell;
    }
}

PelletProjectile::PelletProjectile(
	const Craft::Vector2F& position,
	const Craft::Vector2F& direction,
	float speed,
	float lifeTime,
	bool isHoming)
    : ProjectileBase(position, direction, speed, lifeTime, isHoming)
{
    ChangePixelSprite(CreatePelletSprite());
}

Craft::PixelSprite PelletProjectile::CreatePelletSprite() const
{
    Craft::PixelSprite sprite(1, 1);
    const Craft::ColorRGB pelletColor(255, 220, 80);
    sprite.SetCell(0, 0, MakePixel(pelletColor));

    return sprite;
}
