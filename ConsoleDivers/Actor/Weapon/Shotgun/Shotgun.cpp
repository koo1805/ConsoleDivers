#include "Shotgun.h"
#include <Level/Level.h>
#include <Actor/Projectile/PelletProjectile/PelletProjectile.h>

#include <cmath>


using namespace Craft;

namespace
{
	Craft::Cell MakePixel(const Craft::ColorRGB& color)
	{
		Craft::Cell cell;

		cell.character = ' ';
		cell.foreground = color;
		cell.background = color;

		return cell;
	};
}

Shotgun::Shotgun(const Craft::Vector2F& position)
	: WeaponBase(WeaponSlotType::Primary, WeaponAmmoData{8, 8, 6, 6, 1.8f}, position)
{
    rightPixel = CreateShotgunSprite();

    leftPixel = FlipHorizontal(rightPixel);

	ChangePixelSprite(rightPixel);

    gripOffset = Craft::Vector2F(-2.0f, -2.0f);
}

void Shotgun::StartFire(const Craft::Vector2F& aimDirection)
{
    std::shared_ptr<Craft::Level> level = GetOwner();
    if (!level)
    {
        return;
    }

    std::shared_ptr<Craft::Actor> owner = GetWeaponOwner();
    if (!owner)
    {
        return;
    }

    // 탄약 확인 + 한 발 소비
    if (!ConsumeAmmo())
    {
        return;
    }

    // 중심 발사 방향
    const float angle = std::atan2(aimDirection.y, aimDirection.x);

    const int centerIndex = pelletCount / 2;

    for (int index = 0; index < pelletCount; ++index)
    {
        const int spreadIndex = index - centerIndex;

        const float pelletAngle = angle + static_cast<float>(spreadIndex) * spreadAngle;

        const Craft::Vector2F direction(std::cos(pelletAngle), std::sin(pelletAngle));

        std::shared_ptr<PelletProjectile> projectile = level->SpawnActor<PelletProjectile>(
                GetPosition(),
                direction,
                projectileSpeed,
                projectileLifeTime,
                false
            );

        projectile->SetProjectileOwner(owner);
    }
}

void Shotgun::OnFacingChanged()
{
    if (IsFacingRight())
    {
        // 오른쪽
        ChangePixelSprite(rightPixel);

        gripOffset = Craft::Vector2F(-2.0f, -2.0f);

        return;
    }

    // 왼쪽
    ChangePixelSprite(leftPixel);

    gripOffset = Craft::Vector2F(-8.0f, -2.0f);
}

Craft::PixelSprite Shotgun::CreateShotgunSprite() const
{
	constexpr int width = 11;
	constexpr int height = 3;

	Craft::PixelSprite sprite(width, height);

    // ====================================================
    // Shotgun 색상
    // ====================================================

    // 총 외곽 / 총열
    const Craft::ColorRGB darkMetalColor(35, 38, 36);

    // 금속 밝은 부분
    const Craft::ColorRGB metalColor(80, 84, 78);

    // 총몸
    const Craft::ColorRGB bodyColor(52, 58, 54);

    // 개머리판
    const Craft::ColorRGB stockColor(90, 62, 38);

    // 작은 강조색
    //
    // Helldivers 계열 디자인을 고려한
    // 노란 포인트.
    const Craft::ColorRGB yellowColor(220, 185, 45);

    // ====================================================
    // Color → Cell
    // ====================================================

    const Craft::Cell darkMetalPixel = MakePixel(darkMetalColor);

    const Craft::Cell metalPixel = MakePixel(metalColor);

    const Craft::Cell bodyPixel = MakePixel(bodyColor);

    const Craft::Cell stockPixel = MakePixel(stockColor);

    const Craft::Cell yellowPixel = MakePixel(yellowColor);

    // ====================================================
    // Sprite 데이터
    // ====================================================
    // . = 투명
    //
    // D = Dark Metal
    // M = Metal
    // G = Gun Body
    // S = Stock
    // Y = Yellow Point

    const char* pixels[height] =
    {
        "..GGMMMMMMM",
        "SSGGGDDDDDD",
        ".S.Y......."
    };

    // ====================================================
    // Sprite 변환
    // ====================================================

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            switch (pixels[y][x])
            {
            case 'D':
                sprite.SetCell(x, y, darkMetalPixel);
                break;

            case 'M':
                sprite.SetCell(x, y, metalPixel);
                break;

            case 'G':
                sprite.SetCell(x, y, bodyPixel);
                break;

            case 'S':
                sprite.SetCell(x, y, stockPixel);
                break;

            case 'Y':
                sprite.SetCell(x, y, yellowPixel);
                break;

            case '.':
            default:
                // 아무 Cell도 설정하지 않음.
                //
                // PixelSprite 생성 시 Cell의 character가
                // '\0'으로 초기화되므로 Renderer에서 투명 처리.
                break;
            }
        }
    }

	return sprite;
}

Craft::PixelSprite Shotgun::FlipHorizontal(const Craft::PixelSprite& sprite) const
{
    const int width = sprite.GetWidth();
    const int height = sprite.GetHeight();

    Craft::PixelSprite flippedSprite(width, height);

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            const int sourceX = width - 1 - x;

            flippedSprite.SetCell(x, y, sprite.GetCell(sourceX, y));
        }
    }

    return flippedSprite;
}
