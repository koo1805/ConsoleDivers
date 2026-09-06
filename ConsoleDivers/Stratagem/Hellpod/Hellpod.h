#pragma once

#include <Actor/Actor.h>
#include <Math/Vector2F.h>
#include <Render/Sprite/PixelSprite.h>

#include <Stratagem/Data/StratagemData.h>

// Hellpod 상태
enum class HellpodState
{
	// 착지 위치 위쪽에서 낙하 중
	Falling,

	// 목표 위치 도달
	Landed
};

class Hellpod : public Craft::Actor
{
	TYPE_DECLARATIONS(Hellpod, Actor)

public:
	Hellpod(const Craft::Vector2F& targetPosition, StratagemId stratagemId);

public:
	virtual void Tick(float deltaTime) override;

public:
	inline HellpodState GetHellpodState() const { return state; }

	inline bool IsLanded() const { return state == HellpodState::Landed; }

private:
	// 낙하 처리
	void UpdateFalling(float deltaTime);

	// 착지 처리
	void Land();

	// Hellpod 안의 내용물 생성
	void SpawnPayload();

private:
	Craft::PixelSprite CreateHellpodSprite() const;

private:
	HellpodState state = HellpodState::Falling;

	// 실제 착지 목표 위치
	Craft::Vector2F targetPosition = Craft::Vector2F::Zero;

	// 이 Hellpod이 어떤 스트라타젬 내용물을 가지고 있는지
	StratagemId stratagemId = StratagemId::None;

	// 내용물 중복 Spawn 방지
	bool hasSpawnedPayload = false;

private:
	// 테스트용 낙하 설정
	// ========================================================

	// 목표 지점보다 얼마나 위에서 시작할지
	static constexpr float spawnHeight = 40.0f;

	// 초당 낙하 속도
	static constexpr float fallSpeed = 100.0f;
};