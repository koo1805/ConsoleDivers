#pragma once

#include <Math/Vector2F.h>

#include <memory>

class Player;
class RespawnHUD;

class PlayerRespawnSystem
{
public:
	PlayerRespawnSystem() = default;
	~PlayerRespawnSystem() = default;

public:
	// 시스템 초기화
	void Initialize(const std::shared_ptr<Player>& player, const Craft::Vector2F& respawnPosition);

	// 매 프레임 사망 / Respawn 상태 갱신
	void Update(float deltaTime);

	// Respawn 위치 변경
	void SetRespawnPosition(const Craft::Vector2F& newRespawnPosition);

	void SetRespawnHUD(RespawnHUD* newRespawnHUD);

	// 현재 Respawn 대기 중인지 확인
	inline bool IsWaitingForRespawn() const { return isWaitingForRespawn; }

	// 남은 Respawn 시간
	inline float GetRespawnRemainingTime() const { return respawnTimer; }

private:
	// 실제 Respawn 처리
	void RespawnPlayer();

private:
	// 관리 대상 Player
	std::weak_ptr<Player> player;

	// Player가 다시 생성될 위치
	Craft::Vector2F respawnPosition = Craft::Vector2F::Zero;

	RespawnHUD* respawnHUD = nullptr;

	// 사망 후 Respawn 대기 상태
	bool isWaitingForRespawn = false;

	// 남은 Respawn 시간
	float respawnTimer = 0.0f;

	// 사망 후 Respawn까지 걸리는 시간
	static constexpr float RespawnDelay = 3.0f;
};