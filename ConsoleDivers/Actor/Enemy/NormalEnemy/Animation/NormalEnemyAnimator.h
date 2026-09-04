#pragma once

#include <Actor/Enemy/EnemyAnimatorBase.h>

// 전방 선언
class NormalEnemyVisual;

class NormalEnemyAnimator : public EnemyAnimatorBase
{
public:
	NormalEnemyAnimator() = default;
	virtual ~NormalEnemyAnimator() override = default;

public:
	// 실제 visual연결
	void Initialize(NormalEnemyVisual* visual);

protected:
	virtual int GetFrameCount(EnemyAnimationState state) const override;

	virtual float GetFrameDuration(EnemyAnimationState state) const override;

	virtual void ApplyFrame() override;

	virtual bool IsLooping(EnemyAnimationState state) const override;

private:
	// 소유권 X
	NormalEnemyVisual* visual = nullptr;
};