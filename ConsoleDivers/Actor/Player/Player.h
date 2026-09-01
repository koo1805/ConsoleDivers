#pragma once

#include <Actor/Actor.h>

class Player : public Craft::Actor
{
	TYPE_DECLARATIONS(Player, Actor)

public:
	Player();

public:
	inline bool IsMoving() const { return isMoving; }

private:
	virtual void Tick(float deltaTime) override;

	void PlayerSpriteGenerate();

	void Move(float xDirection, float yDirection, float deltaTime);

private:
	bool isMoving = false;

	// 이동 처리에 필요함 변수

	// 이동 속도 변수
	float moveSpeed = 80.0f;
};

