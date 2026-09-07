#include "PlayerRespawnSystem.h"
#include <Actor/Player/Player.h>

void PlayerRespawnSystem::Initialize(const std::shared_ptr<Player>& player, const Craft::Vector2F& respawnPosition)
{
	this->player = player;

	this->respawnPosition = respawnPosition;

	isWaitingForRespawn = false;

	respawnTimer = 0.0f;
}

void PlayerRespawnSystem::Update(float deltaTime)
{
	std::shared_ptr<Player> currentPlayer = player.lock();

	// 예외 처리
	if (!currentPlayer)
	{
		return;
	}

	// 아직 리스폰 대기 시간이 아닐때
	if (!isWaitingForRespawn)
	{
		// 살아 있음
		if (!currentPlayer->IsDead())
		{
			return;
		}

		// player 사망
		isWaitingForRespawn = true;

		respawnTimer = RespawnDelay;

		return;
	}

	// 리스폰 대기 중
	respawnTimer -= deltaTime;

	// 시간이 남아있다면 대기
	if (respawnTimer > 0.0f)
	{
		return;
	}

	// 시간 종료
	respawnTimer = 0.0f;

	RespawnPlayer();
}

void PlayerRespawnSystem::SetRespawnPosition(const Craft::Vector2F& newRespawnPosition)
{
	respawnPosition = newRespawnPosition;
}

void PlayerRespawnSystem::RespawnPlayer()
{
	std::shared_ptr<Player> currentPlayer = player.lock();

	if (!currentPlayer)
	{
		isWaitingForRespawn = false;

		respawnTimer = 0.0f;

		return;
	}

	// 리스폰 초기화 함수
	currentPlayer->Respawn(respawnPosition);

	isWaitingForRespawn = false;

	respawnTimer = 0.0f;
}