#include "ArcEffect.h"

#include <Render/Cell.h>

#include <algorithm>
#include <cmath>

ArcEffect::ArcEffect(const Craft::Vector2F& startPosition, const Craft::Vector2F& endPosition)
    : Craft::Actor(startPosition)
{
    // 두 World 위치를 연결하는 PixelSprite 생성
    Craft::PixelSprite arcSprite = CreateArcSprite(startPosition, endPosition);

    ChangePixelSprite(arcSprite);

    // ArcEffect의 실제 Actor 위치는 두 점을 감싸는 Bounding Box의 왼쪽 위
    const float minX = (std::min)(startPosition.x, endPosition.x);
    const float minY = (std::min)(startPosition.y, endPosition.y);

    SetPosition(Craft::Vector2F(std::floor(minX), std::floor(minY)));

    // 캐릭터 / 무기보다 위에 표시
    sortingOrder = 850;
}


void ArcEffect::Tick(float deltaTime)
{
    // Actor 기본 Tick
    Craft::Actor::Tick(deltaTime);

    lifeTime += deltaTime;

    // Arc는 실제 Projectile이 아니므로
    // 약 0.1초만 표시한 뒤 바로 제거
    if (lifeTime >= maxLifeTime)
    {
        Destroy();
    }
}


Craft::PixelSprite ArcEffect::CreateArcSprite(const Craft::Vector2F& startPosition, const Craft::Vector2F& endPosition)
{
    // 두 World 위치를 포함하는 Bounding Box 계산
    // ============================================================
    const int minX = static_cast<int>(std::floor((std::min)(startPosition.x, endPosition.x)));
    const int minY = static_cast<int>(std::floor((std::min)(startPosition.y, endPosition.y)));

    const int maxX = static_cast<int>(std::ceil((std::max)(startPosition.x, endPosition.x)));
    const int maxY = static_cast<int>(std::ceil((std::max)(startPosition.y, endPosition.y)));

    const int width = (std::max)(1, maxX - minX + 1);
    const int height = (std::max)(1, maxY - minY + 1);

    Craft::PixelSprite sprite(width, height);

    // World 좌표 -> Sprite Local 좌표
    // ============================================================
    int x0 = static_cast<int>(std::round(startPosition.x)) - minX;
    int y0 = static_cast<int>(std::round(startPosition.y)) - minY;

    const int x1 = static_cast<int>(std::round(endPosition.x)) - minX;
    const int y1 = static_cast<int>(std::round(endPosition.y)) - minY;

    // 전기 Cell
    // ============================================================
    Craft::Cell electricCell;

    electricCell.character = ' ';
    electricCell.foreground = Craft::ColorRGB(120, 230, 255);
    electricCell.background = Craft::ColorRGB(120, 230, 255);

    Craft::Cell brightElectricCell;

    brightElectricCell.character = ' ';
    brightElectricCell.foreground = Craft::ColorRGB(220, 250, 255);
    brightElectricCell.background = Craft::ColorRGB(220, 250, 255);

    // Bresenham
    // ============================================================
    const int deltaX = std::abs(x1 - x0);
    const int stepX = (x0 < x1) ? 1 : -1;

    const int deltaY = -std::abs(y1 - y0);
    const int stepY = (y0 < y1) ? 1 : -1;

    int error = deltaX + deltaY;

    int pixelIndex = 0;

    while (true)
    {
        // 한 색으로만 표시하면 단순한 Laser처럼 보이므로
        // 두 가지 밝기를 번갈아 사용해서 전기 느낌을 줌
        const Craft::Cell& currentCell = ((pixelIndex % 3) == 0) ? brightElectricCell : electricCell;

        sprite.SetCell(x0, y0, currentCell);

        ++pixelIndex;

        if (x0 == x1 && y0 == y1)
        {
            break;
        }

        const int doubledError = error * 2;

        if (doubledError >= deltaY)
        {
            error += deltaY;

            x0 += stepX;
        }

        if (doubledError <= deltaX)
        {
            error += deltaX;

            y0 += stepY;
        }
    }

    return sprite;
}