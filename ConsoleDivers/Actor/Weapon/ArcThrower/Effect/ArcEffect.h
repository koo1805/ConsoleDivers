#pragma once

#include <Actor/Actor.h>

class ArcEffect : public Craft::Actor
{
    TYPE_DECLARATIONS(ArcEffect, Craft::Actor)

public:
    ArcEffect(const Craft::Vector2F& startPosition, const Craft::Vector2F& endPosition);

    virtual ~ArcEffect() override = default;

public:
    // 짧은 시간 후 자동 제거
    virtual void Tick(float deltaTime) override;

private:
    // 시작점과 끝점으로 번개 Sprite 생성
    Craft::PixelSprite CreateArcSprite(const Craft::Vector2F& startPosition, const Craft::Vector2F& endPosition);

private:
    // 번개가 화면에 유지되는 시간
    float lifeTime = 0.0f;

    static constexpr float maxLifeTime = 0.10f;
};