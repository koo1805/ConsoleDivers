#pragma once

#include <Render/Sprite/PixelSprite.h>
#include <Math/Vector2.h>

#include <vector>

// Player 애니메이션 상태
enum class PlayerAnimationState
{
    Idle,
    Walk,
    Aim,
    Fire
};

// 한 CharacterPart의 한 프레임 정보
struct CharacterPartFrame
{
    Craft::PixelSprite sprite;

    Craft::Vector2 localPosition =  Craft::Vector2::Zero;

    int sortingOffset = 0;
};

// 한 부위의 애니메이션
struct CharacterPartAnimation
{
    std::vector<CharacterPartFrame> frames;

    float frameDuration = 0.1f;

    bool loop = true;
};