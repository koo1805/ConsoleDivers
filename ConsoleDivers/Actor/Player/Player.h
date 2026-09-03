#pragma once

#include <Actor/Character/Character.h>
#include <Actor/Player/Sprite/PlayerVisual.h>
#include <Actor/Player/Animation/PlayerAnimator.h>

class Player : public Character
{
	TYPE_DECLARATIONS(Player, Character)

public:
	Player();

public:
	inline bool IsMoving() const { return isMoving; }

	// 현재 바라보는 방향
	inline bool IsFacingRight() const { return isFacingRight; }

	// 현재 다이브 중인지 확인
	inline bool IsDiving() const { return isDiving; }

	// 현재 무적인지 확인
	inline bool IsInvincible() const { return isInvincible; }

private:
	virtual void Tick(float deltaTime) override;

	void PlayerPartsGenerate();

	void Move(float xDirection, float yDirection, float deltaTime);

	// 현재 Player의 화면상 위치와 마우스 화면 위치를 비교해서 좌/우 방향 결정
	void UpdateFacingDirection();

	// 다이브 시작
	void StartDive(float xDirection, float yDirection);

	// 다이브중 시간 처리
	void UpdateDive(float deltaTime);

	// 다이브 종료
	void EndDive();

private:
	bool isMoving = false;

	// true  : 오른쪽 | false : 왼쪽
	bool isFacingRight = true;

	// 다이브 상태 확인 변수
	bool isDiving = false;

	// 무적 상태 확인 변수
	bool isInvincible = false;

	// 다이브 진행 시간
	float diveTimer = 0.0f;

	// 무적 시간
	float invincibleTimer = 0.0f;

	// 다이브 이동 방향
	Craft::Vector2F diveDirection = Craft::Vector2F::Zero;

	PlayerVisual visual;

	PlayerAnimator animator;

	// 기존 Player의 전체 크기 유지
	// Character가 여러 Sprite로 분리되더라도 = 카메라 / 월드 Bounds 계산에서는 기존 9 x 15 크기로 동작
	static constexpr int PlayerWidth = 9;
	static constexpr int PlayerHeight = 15;

	// 대각선 정규화
	static constexpr float diagonalScale = 0.7071f;

	// 다이브 이동 속도
	static constexpr float diveSpeed = 180.0f;

	// 다이브 지속 시간
	static constexpr float diveDuration = 0.35f;

	// 다이브 시작후 무적 유지 시간
	static constexpr float diveInvincibleDuration = 0.25f;
};

