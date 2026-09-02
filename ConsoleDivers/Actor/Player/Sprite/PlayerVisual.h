#pragma once

#include <Actor/Player/Data/PlayerVisualData.h>

// 전방 선언
class CharacterPart;

// Player의 화면 표현 담당
class PlayerVisual
{
public:
    PlayerVisual() = default;

public:
    void Initialize(
        CharacterPart* head,
        CharacterPart* body,
        CharacterPart* legs,
        CharacterPart* leftHand,
        CharacterPart* rightHand);

    void Tick(float deltaTime);

    void SetFacingRight(bool facingRight);

    void SetAnimationState(PlayerAnimationState state);

    inline bool IsFacingRight() const { return isFacingRight; }

private:
    // Visual Data 생성
    void GenerateVisualData();

    void GenerateIdleAnimation();

    void GenerateWalkAnimation();
    // 추후
    void GenerateAimAnimation();
    void GenerateFireAnimation();

private:
    // 애니메이션 처리
    void UpdateAnimation(float deltaTime);

    void ApplyCurrentFrame();

    void ApplyFacing();


private:
    Craft::PixelSprite FlipHorizontal(const Craft::PixelSprite& sprite) const;


private:
    // 실제 CharacterPart - 소유권 없음
    CharacterPart* head = nullptr;
    CharacterPart* body = nullptr;
    CharacterPart* legs = nullptr;

    CharacterPart* leftHand = nullptr;
    CharacterPart* rightHand = nullptr;


private:
    bool isInitialized = false;

    bool isFacingRight = true;

    PlayerAnimationState currentState = PlayerAnimationState::Idle;

    int currentFrame = 0;

    float frameTimer = 0.0f;

private:
    // Player Visual Data
    // 현재는 Idle만 실제 데이터가 들어간다.

    CharacterPartAnimation idleHead;
    CharacterPartAnimation idleBody;
    CharacterPartAnimation idleLegs;

    CharacterPartAnimation idleLeftHand;
    CharacterPartAnimation idleRightHand;

    // 추후
    CharacterPartAnimation walkHead;
    CharacterPartAnimation walkBody;
    CharacterPartAnimation walkLegs;

    CharacterPartAnimation walkLeftHand;
    CharacterPartAnimation walkRightHand;
};

