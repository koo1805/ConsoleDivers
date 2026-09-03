#pragma once

#include <Actor/Weapon/WeaponBase.h>

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

private:
    void FireChargedShot(const Craft::Vector2F& aimDirection);
    
private:
    // 현재 충전 중
    bool isCharging = false;

    // 현재 충전 시간
    float chargeTime = 0.0f;

    // 이 시간 이상 충전해야 발사 가능
    static constexpr float minimumChargeTime = 0.8f;

    // 충전 중 이동속도
    static constexpr float chargeMoveMultiplier = 0.45f;

    // 임시 Pellet
    static constexpr int pelletCount = 5;
    static constexpr float projectileSpeed = 60.0f;
    static constexpr float projectileLifeTime = 1.5f;
};

