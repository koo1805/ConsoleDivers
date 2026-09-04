#include "NormalEnemyAnimator.h"
#include <Actor/Enemy/NormalEnemy/Sprite/NormalEnemyVisual.h>

void NormalEnemyAnimator::Initialize(NormalEnemyVisual* visual)
{
	this->visual = visual;

	if (!this->visual)
	{
		return;
	}

	// 기본 상태 애니메이션의 첫 프레임 적용
	ApplyFrame();
}

int NormalEnemyAnimator::GetFrameCount(EnemyAnimationState state) const
{
	switch (state)
	{
	case EnemyAnimationState::Idle:
		return 1;

	case EnemyAnimationState::Walk:
		return 2;

	case EnemyAnimationState::Attack:
		return 2;

	case EnemyAnimationState::Death:
		return 1;
	}

	return 1;
}

float NormalEnemyAnimator::GetFrameDuration(EnemyAnimationState state) const
{
	switch (state)
	{
	case EnemyAnimationState::Idle:
		return 0.5f;

	case EnemyAnimationState::Walk:
		return 0.15f;

	case EnemyAnimationState::Attack:
		return 0.2f;

	case EnemyAnimationState::Death:
		return 0.8f;
	}
	return 0.2f;
}

void NormalEnemyAnimator::ApplyFrame()
{
	if (!visual)
	{
		return;
	}

	switch (GetAnimationState())
	{
	case EnemyAnimationState::Idle:
		visual->SetIdleFrame(GetCurrentFrame());
		break;

	case EnemyAnimationState::Walk:
		visual->SetWalkFrame(GetCurrentFrame());
		break;

	case EnemyAnimationState::Attack:
		visual->SetAttackFrame(GetCurrentFrame());
		break;

	case EnemyAnimationState::Death:
		visual->SetDeathFrame(GetCurrentFrame());
		break;
	}
}

bool NormalEnemyAnimator::IsLooping(EnemyAnimationState state) const
{
	switch (state)
	{
	case EnemyAnimationState::Idle:
		return true;

	case EnemyAnimationState::Walk:
		return true;

	case EnemyAnimationState::Attack:
		return false;

	case EnemyAnimationState::Death:
		return false;
	}
	return true;
}
