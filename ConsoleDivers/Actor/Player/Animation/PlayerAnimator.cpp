#include "PlayerAnimator.h"
#include <Actor/Player/Sprite/PlayerVisual.h>

void PlayerAnimator::Initialized(PlayerVisual* visual)
{
	this->visual = visual;

	currentState = PlayerAnimationState::Idle;

	currentFrame = 0;

	frameTimer = 0.0f;

	isInitialized = true;

	// Idle상태로 초기화
	ApplyCurrentFrame();
}

void PlayerAnimator::Tick(float deltaTime)
{
	if (!isInitialized || !visual)
	{
		return;
	}

	// 현재 애니메이션의 프레임 수
	const int frameCount = visual->GetAnimationFrameCount(currentState);

	if (frameCount <= 0)
	{
		return;
	}

	// 현재 애니메이션 프레임의 유지 시간
	const float frameDuration = visual->GetAnimationFrameDuration(currentState);

	if (frameDuration <= 0)
	{
		return;
	}

	// 시간 누적
	frameTimer += deltaTime;

	while (frameTimer >= frameDuration)
	{
		frameTimer -= frameDuration;

		++currentFrame;

		// 마지막 프레임 처리
		if (currentFrame >= frameCount)
		{
			// 반복 애니메이션
			if (visual->IsAnimationLooping(currentState))
			{
				currentFrame = 0;
			}
			else		// 반복하지 않는 애니메이션이면 마지막 프레임 유지
			{
				currentFrame = frameCount - 1;
				frameTimer = 0.0f;
				break;
			}
		}

		// 변경된 프레임 적용
		ApplyCurrentFrame();
	}
}

void PlayerAnimator::SetAnimationState(PlayerAnimationState state)
{
	if (!isInitialized)
	{
		return;
	}

	// 동일한 상태라면 프레임을 처음부터 다시 시작하지 않음
	if (currentState == state)
	{
		return;
	}

	currentState = state;

	// 새로운 애니메이션은 첫 프레임부터 시작
	currentFrame = 0;

	frameTimer = 0.0f;

	ApplyCurrentFrame();
}

// 방향 변경 등 애니메이션 프레임은 그대로 두고 화면 표현만 다시 계산할 때 사용
void PlayerAnimator::RedrawCurrentFrame()
{
	if (!isInitialized)
	{
		return;
	}
	
	ApplyCurrentFrame();
}

void PlayerAnimator::ApplyCurrentFrame()
{
	if (!isInitialized)
	{
		return;
	}

	visual->ApplyAnimationFrame(currentState, currentFrame);
}
