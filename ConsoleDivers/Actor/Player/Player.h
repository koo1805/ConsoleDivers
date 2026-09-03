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

private:
	virtual void Tick(float deltaTime) override;

	void PlayerPartsGenerate();

	void Move(float xDirection, float yDirection, float deltaTime);

	// 현재 Player의 화면상 위치와 마우스 화면 위치를 비교해서 좌/우 방향 결정
	void UpdateFacingDirection();

private:
	bool isMoving = false;

	// true  : 오른쪽 | false : 왼쪽
	bool isFacingRight = true;

	PlayerVisual visual;

	PlayerAnimator animator;

	// 기존 Player의 전체 크기 유지
	// Character가 여러 Sprite로 분리되더라도 = 카메라 / 월드 Bounds 계산에서는 기존 9 x 15 크기로 동작
	static constexpr int PlayerWidth = 9;
	static constexpr int PlayerHeight = 15;
};

