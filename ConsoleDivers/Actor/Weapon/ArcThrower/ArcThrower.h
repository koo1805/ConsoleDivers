#pragma once

#include <Actor/Weapon/WeaponBase.h>

// 전방 선언
class EnemyBase;
class NavigationGrid;

namespace Craft
{
    class QuadTree;
}

class ArcThrower : public WeaponBase
{
    TYPE_DECLARATIONS(ArcThrower, WeaponBase)

public:
    ArcThrower(const Craft::Vector2F& position = Craft::Vector2F::Zero);

    virtual ~ArcThrower() override = default;

public:
    virtual void StartFire(const Craft::Vector2F& aimDirection) override;

    virtual void UpdateFire(float deltaTime, const Craft::Vector2F& aimDirection) override;

    virtual void ReleaseFire(const Craft::Vector2F& aimDirection) override;

    virtual float GetMoveSpeedMultiplier() const override;

    virtual void CancelFire() override;

    virtual bool IsCharging() const override { return isCharging; }

    virtual float GetChargeProgress() const override;

    // HUD 슬롯
    virtual const Craft::PixelSprite* GetHUDSprite() const override { return &rightPixel; }

    void SetQuadTree(const Craft::QuadTree* newQuadTree) { quadTree = newQuadTree; }

    // ArcThrower 벽 판정에 사용할 NavigationGrid 연결
    void SetNavigationGrid(const NavigationGrid* newNavigationGrid) { navigationGrid = newNavigationGrid; }

protected:
    // Player 방향이 바뀌었을 때 무기 Sprite도 좌/우 전환
    virtual void OnFacingChanged() override;

private:
    // 실제 Chain Lightning 발사
    void FireChainLightning(const Craft::Vector2F& aimDirection);

    // 첫 타겟 검색
    std::shared_ptr<EnemyBase> FindInitialTarget(const Craft::Vector2F& origin, const Craft::Vector2F& aimDirection) const;

    // Chain의 다음 타겟 검색
    std::shared_ptr<EnemyBase> FindNextChainTarget(
            const std::shared_ptr<EnemyBase>& currentTarget,
            const std::vector<std::shared_ptr<EnemyBase>>& alreadyHitTargets) const;

    // Enemy 실제 Damage
    void ApplyElectricDamage(const std::shared_ptr<EnemyBase>& target) const;

    // 이미 맞은 Enemy인지 검사
    bool HasAlreadyHit(
        const std::shared_ptr<EnemyBase>& target,
        const std::vector<std::shared_ptr<EnemyBase>>& alreadyHitTargets) const;

    // 두 위치 사이에 벽이 존재하지 않는지 검사
    bool HasLineOfSight(const Craft::Vector2F& startPosition, const Craft::Vector2F& endPosition) const;

    // Arc가 실제로 발사되는 총구 위치
    Craft::Vector2F GetArcMuzzlePosition() const;

    // Enemy의 Arc 타격 기준 위치 계산
    Craft::Vector2F GetTargetCenter(const std::shared_ptr<EnemyBase>& target) const;

    Craft::Vector2F GetArcVisualEndPosition(const Craft::Vector2F& startPosition, const Craft::Vector2F& aimDirection) const;

    // ArcThrower 기본 Sprite 생성
    Craft::PixelSprite CreateArcThrowerSprite() const;

    // 좌우 반전 Sprite 생성
    Craft::PixelSprite FlipHorizontal(const Craft::PixelSprite& sprite) const;
    
private:
    Craft::PixelSprite rightPixel;
    Craft::PixelSprite leftPixel;

    // GameLevel이 소유
    const Craft::QuadTree* quadTree = nullptr;

    // 현재 충전 중
    bool isCharging = false;

    // 현재 충전 시간
    float chargeTime = 0.0f;

    // 벽 / 이동 가능 Grid 정보
    const NavigationGrid* navigationGrid = nullptr;

    // 이 시간 이상 충전해야 발사 가능
    static constexpr float minimumChargeTime = 0.8f;

    // 충전 중 이동속도
    static constexpr float chargeMoveMultiplier = 0.45f;

    // Chain Lightning 설정
    // 최초 적 탐색 거리
    static constexpr float initialTargetRange = 80.0f;

    // 첫 적을 찾을 때 허용하는 전방 Cone
    // Dot 1.0 = 정확히 정면 | Dot 0.7 정도면 약 ±45도 영역
    static constexpr float minimumAimDot = 0.70f;

    // 한 Enemy에서 다음 Enemy로 튈 수 있는 거리
    static constexpr float chainRange = 45.0f;

    // 최초 Target 포함 최대 적중 수
    static constexpr int maxChainTargets = 4;
};

