#pragma once

#include <Actor/Enemy/EnemyVisualBase.h>
#include <array>

// 전방 선언
class CharacterPart;

class NormalEnemyVisual : public EnemyVisualBase
{
public:
	NormalEnemyVisual() = default;
	virtual ~NormalEnemyVisual() override = default;

public:
	void Initialize(CharacterPart* body);

public:
	void SetIdleFrame(int frameIndex);
	void SetWalkFrame(int frameIndex);
	void SetAttackFrame(int frameIndex);
	void SetDeathFrame(int frameIndex);

protected:
	virtual void ApplyFacing() override;

private:
	// Sprite 데이터 생성
	void GenerateVisualData();

	Craft::PixelSprite CreateIdleSprite() const;
	Craft::PixelSprite CreateWalkSprite(int frameIndex) const;
	Craft::PixelSprite CreateAttackSprite(int frameIndex) const;
	Craft::PixelSprite CreateDeathSprite() const;

private:
	CharacterPart* body = nullptr;

	Craft::PixelSprite bodySprite;

	Craft::PixelSprite idleSprite;
	std::array<Craft::PixelSprite, 2> walkSprites;
	std::array<Craft::PixelSprite, 2> attackSprites;
	Craft::PixelSprite deathSprite;
};

