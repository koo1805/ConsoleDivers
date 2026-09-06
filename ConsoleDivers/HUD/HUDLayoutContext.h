#pragma once

#include <Math/Vector2.h>

// Game HUD 배치에 필요한 화면 정보
struct HUDLayoutContext
{
    // HUD까지 포함한 전체 화면
    Craft::Vector2 screenSize = Craft::Vector2::Zero;

    // 실제 게임 월드가 출력되는 영역
    Craft::Vector2 viewportSize = Craft::Vector2::Zero;
};