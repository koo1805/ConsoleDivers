#pragma once

#include <Render/Sprite/PixelSprite.h>

class EnemyVisualBase
{
public:
	EnemyVisualBase() = default;
	virtual ~EnemyVisualBase() = default;

public:
	// 방향이 변경된 경우 파생 클래스에 요청
	void SetFacingRight(bool facingRight);

protected:
	// 파생 클래스의 Initialize 마지막에 호출
	void SetInitialized(bool initialized);

	// 현재 초기화 여부
	inline bool IsInitialized() const { return isInitialized; }

	// 현재 방향
	inline bool IsFacingRight() const { return isfacingRight; }

	// 각 파생 클래스마다 부위가 다르므로 파생 클래스에서 각자 구현
	virtual void ApplyFacing() = 0;

	// 좌우 반전
	Craft::PixelSprite FlipHorizontal(const Craft::PixelSprite& sprite) const;

private:
	// Visual이 CharacterPart와 연결되었는지 여부
	bool isInitialized = false;

	// 현재 바라보는 방향	| true - 오른쪽
	bool isfacingRight = true;

};

