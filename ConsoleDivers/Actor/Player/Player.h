#pragma once

#include <Actor/Character/Character.h>

class Player : public Character
{
	TYPE_DECLARATIONS(Player, Character)

public:
	Player();

public:
	inline bool IsMoving() const { return isMoving; }

private:
	virtual void Tick(float deltaTime) override;

	void PlayerPartsGenerate();

	void Move(float xDirection, float yDirection, float deltaTime);

private:
	bool isMoving = false;

	// 기존 Player의 전체 크기 유지
	// Character가 여러 Sprite로 분리되더라도 = 카메라 / 월드 Bounds 계산에서는 기존 9 x 15 크기로 동작
	static constexpr int PlayerWidth = 9;
	static constexpr int PlayerHeight = 15;
};

