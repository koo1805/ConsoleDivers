#pragma once

enum class EnemyAnimationState
{
	Idle,
	Walk,
	Attack,
	Death
};

class EnemyAnimatorBase
{
public:
	EnemyAnimatorBase() = default;
	virtual ~EnemyAnimatorBase() = default;

public:
	// 매 프레임 애니메이션 갱신
	void Tick(float deltaTime);

	// 애니메이션 상태 변경
	void SetAnimationState(EnemyAnimationState newState);

	inline bool IsAnimationFinished() const { return animationFinished; }

	inline int GetCurrentAnimationFrame() const { return currentFrame; }

protected:
	inline EnemyAnimationState GetAnimationState() const { return animationState; }

	inline int GetCurrentFrame() const { return currentFrame; }

	// 애니메이션 설정 - 파생 클래스에서 frameCount / frameDuration설정
	virtual int GetFrameCount(EnemyAnimationState state) const = 0;

	virtual float GetFrameDuration(EnemyAnimationState state) const = 0;

	// 현재 프레임을 실제 Visual에 적용
	virtual void ApplyFrame() = 0;

	// 반복 여부
	virtual bool IsLooping(EnemyAnimationState state) const = 0;

private:
	EnemyAnimationState animationState = EnemyAnimationState::Idle;

	int currentFrame = 0;

	float frameTimer = 0.0f;

	bool animationFinished = false;
};

