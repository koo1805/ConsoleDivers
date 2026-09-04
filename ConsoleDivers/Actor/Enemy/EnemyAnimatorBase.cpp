#include "EnemyAnimatorBase.h"

void EnemyAnimatorBase::Tick(float deltaTime)
{
	const int frameCount = GetFrameCount(animationState);

	// 프레임이 하나뿐이면 건너뛰기
	if (frameCount <= 0)
	{
		return;
	}

	const float frameDuration = GetFrameDuration(animationState);

	if (frameDuration <= 0.0f)
	{
		return;
	}

	// 1 프레임 일때
	if (frameCount == 1)
	{
		if (IsLooping(animationState))
		{
			return;
		}

		frameTimer += deltaTime;

		if (frameTimer >= frameDuration)
		{
			animationFinished = true;
		}

		return;
	}

	// 2 프레임 일때
	// 시간 누적
	frameTimer += deltaTime;

	if (frameTimer < frameDuration)
	{
		return;
	}

	frameTimer -= frameDuration;
	++currentFrame;

	// 마지막 프레임 이후 처리
	if (currentFrame >= frameCount)
	{
		if (IsLooping(animationState))
		{
			// 반복
			currentFrame = 0;
		}
		else
		{
			// 마지막 프레임 유지
			currentFrame = frameCount - 1;

			// 애니메이션 종료
			animationFinished = true;
		}
	}

	// 변경된 프레임 적용
	ApplyFrame();
}

void EnemyAnimatorBase::SetAnimationState(EnemyAnimationState newState)
{
	// 같은 상태 건너뛰기
	if (animationState == newState)
	{
		return;
	}

	animationState = newState;

	// 상태 변경시 애니메이션 초기화
	currentFrame = 0;
	frameTimer = 0.0f;

	// 새 애니메이션 시작
	animationFinished = false;

	// 새 상태의 첫 프레임 적용
	ApplyFrame();
}
