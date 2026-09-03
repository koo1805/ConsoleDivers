#pragma once

#include <Actor/Player/Data/PlayerVisualData.h>

// 전방 선언
class PlayerVisual;

class PlayerAnimator
{
public:
	PlayerAnimator() = default;

public:
	// 실제 화면 표현을 담당하는 PlayerVisual 연결 - 소유권X
	void Initialized(PlayerVisual* visual);

	void Tick(float deltaTime);

	// 애니메이션의 상태 변경
	void SetAnimationState(PlayerAnimationState state);

	// 프레임별 sprite 다시 그리기
	void RedrawCurrentFrame();

	// Getter
	inline PlayerAnimationState GetCurrentState() const { return currentState; }

	inline int GetCurrentFrame() const { return currentFrame; }

private:
	// 현재 상태의 프레임을 PlayerVisual에 적용
	void ApplyCurrentFrame();

private:
	// PlayerVisual은 Player가 소유 - Animator는 주소만 참조
	PlayerVisual* visual = nullptr;

	PlayerAnimationState currentState = PlayerAnimationState::Idle;

	int currentFrame = 0;

	float frameTimer = 0.0f;

	bool isInitialized = false;
};