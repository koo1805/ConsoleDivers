#pragma once

#include <Actor/Actor.h>

#include <memory>

class ProjectileBase : public Craft::Actor
{
	TYPE_DECLARATIONS(ProjectileBase, Actor)

public:
	ProjectileBase(
		const Craft::Vector2F& position,
		const Craft::Vector2F& direction,
		float speed,
		float lifeTime,
		bool isHoming = false
	);
	virtual ~ProjectileBase() override = default;

public:
	virtual void Tick(float deltaTime) override;

	// 진행 방향 설정
	void SetDirection(const Craft::Vector2F& newDirection);

	// Get Owner - 발사자를 기억할 때 사용
	void SetProjectileOwner(const std::shared_ptr<Craft::Actor>& newOwner);

	// Getter
	inline Craft::Vector2F GetDirection() const { return direction; }
	inline bool IsHoming() const { return isHoming; }
	inline std::shared_ptr<Craft::Actor> GetProjectileOwner() const { return projectileOwner.lock(); }

	// Setter
	inline void SetSpeed(float newSpeed) { speed = newSpeed; }

protected:
	virtual void Move(float deltaTime);

	void UpdateLifeTime(float deltaTime);

private:
	// 정규화된 방향
	Craft::Vector2F direction = Craft::Vector2F::Zero;

	// 이동 속도
	float speed = 0.0f;

	// 생성후 흐른 시간
	float elapsedLifeTime = 0.0f;

	// 수명
	float lifeTime = 0.0f;

	// 유도 여부
	bool isHoming = false;

	// projectile을 발사한 액터
	std::weak_ptr<Craft::Actor> projectileOwner;


};

