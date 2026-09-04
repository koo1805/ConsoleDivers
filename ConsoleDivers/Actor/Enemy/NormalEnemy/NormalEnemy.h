#pragma once

#include <Actor/Enemy/EnemyBase.h>
#include <Actor/Enemy/NormalEnemy/Sprite/NormalEnemyVisual.h>
#include <Actor/Enemy/NormalEnemy/Animation/NormalEnemyAnimator.h>

class NormalEnemy : public EnemyBase
{
	TYPE_DECLARATIONS(NormalEnemy, EnemyBase)

public:
	NormalEnemy(const Craft::Vector2F& position = Craft::Vector2F::Zero);
	virtual ~NormalEnemy() override = default;

public:
	virtual void BeginPlay() override;

	virtual void Tick(float deltaTime) override;

	virtual void OnDeath() override;

private:
	void MeleeAttack();

	void UpdateAttack();

	void UpdateBehavior(float deltaTime);

	void UpdateVisual();

	void UpdateAnimationState();

private:
	NormalEnemyVisual visual;
	NormalEnemyAnimator animator;

	// 이미 데미지를 줬는지 판단
	bool hasAppliedAttackDamage = false;

	static constexpr int EnemyWidth = 7;
	static constexpr int EnemyHeight = 7;
};

