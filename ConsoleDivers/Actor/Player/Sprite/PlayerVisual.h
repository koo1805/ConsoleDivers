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

    // 바라보는 방향 설정
    void SetFacingRight(bool facingRight);

    inline bool IsFacingRight() const { return isFacingRight; }

    // 해당 애니메이션 프레임 수
    int GetAnimationFrameCount(PlayerAnimationState state) const;

    // 해당 애니메이션의 프레임 유지 시간
    float GetAnimationFrameDuration(PlayerAnimationState state) const;

    // 해당 애니메이션이 반복되는지 확인
    bool IsAnimationLooping(PlayerAnimationState state) const;

    // Animator가 선택한 프레임을 캐릭터 부위에 적용
    void ApplyAnimationFrame(PlayerAnimationState state, int frameIndex);

private:
    // Visual Data 생성
    void GenerateVisualData();

    void GenerateIdleAnimation();

    void GenerateWalkAnimation();
    // 추후
    void GenerateAimAnimation();
    void GenerateFireAnimation();

private:
    // 현재 애니메이션 프레임 하나를 캐릭터 부위에 적용
    void ApplyPartFrame(CharacterPart* part, const CharacterPartAnimation* animation, int frameIndex);

    // 현재 방향에 따른 좌우 반전
    void ApplyFacing();

    // Sprite 좌우 반전
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

    // true - 오른쪽
    bool isFacingRight = true;

    static constexpr int playerWidth = 9;

private:
    // Player Visual Data
    
    // Idle
    CharacterPartAnimation idleHead;
    CharacterPartAnimation idleBody;
    CharacterPartAnimation idleLegs;
    CharacterPartAnimation idleLeftHand;
    CharacterPartAnimation idleRightHand;

    // Walk
    CharacterPartAnimation walkHead;
    CharacterPartAnimation walkBody;
    CharacterPartAnimation walkLegs;
    CharacterPartAnimation walkLeftHand;
    CharacterPartAnimation walkRightHand;
};

