#pragma once

#include <Math/Vector2.h>
#include <Math/Vector2F.h>

#include <array>
#include <memory>

// 전방 선언
class NavigationGrid;
class EnemyBase;

namespace Craft
{
	class Level;
}

// 게임 레벨에서 Enemy 생성을 관리
class EnemySpawnManager
{
private:
	// 생성 가능한 Enemy 종류
	enum class EnemyType
	{
		Normal
	};

	// Viewport 기준 Spawn 방향
	enum class SpawnSide
	{
		Left,
		Right,
		Top,
		Bottom
	};

public:
	EnemySpawnManager() = default;
	~EnemySpawnManager() = default;

public:
	// SpawnManager 초기화
	void Initialize(Craft::Level& level, NavigationGrid& navigationGrid, const Craft::Vector2& worldSize);

	// 매 프레임 Spawn 상태 갱신
	void Tick(float deltaTime);

private:
	// 현재 살아있는 Enemy 수 계산
	int GetAliveEnemyCount();

	// Enemy 하나 생성 시도
	bool TrySpawnEnemy();

	// 실제 Enemy 종류에 따라 Actor 생성
	std::shared_ptr<EnemyBase> SpawnEnemy(EnemyType enemyType, const Craft::Vector2F& spawnPosition);

	// 유효한 Spawn 위치 찾기
	bool TryFindSpawnPosition(Craft::Vector2F& outSpawnPosition);

	// 선택된 Viewport 방향 기준
	// 랜덤 Spawn 위치 생성
	Craft::Vector2F MakeRandomSpawnPosition(SpawnSide spawnSide) const;

	// 최종 Spawn 위치 검증
	bool IsSpawnPositionValid(const Craft::Vector2F& spawnPosition) const;

private:
	// Enemy를 생성할 Level
	Craft::Level* level = nullptr;

	// A* NavigationGrid
	NavigationGrid* navigationGrid = nullptr;

	// 월드 전체 크기
	Craft::Vector2 worldSize = Craft::Vector2::Zero;

	// 현재 Spawn 타이머
	float spawnTimer = 0.0f;

	// 현재 생성 가능한 Enemy 배열
	std::array<EnemyType, 1> enemyTypes =
	{
		EnemyType::Normal
	};

	// Enemy Spawn 주기
	static constexpr float SpawnInterval = 5.0f;

	// 동시에 존재 가능한 최대 살아있는 Enemy 수
	static constexpr int MaxAliveEnemies = 10;

	// Spawn 위치 탐색 최대 횟수
	static constexpr int MaxSpawnPositionAttempts = 32;

	// Viewport 가장자리로부터 얼마나 떨어져 Spawn할지
	static constexpr float SpawnOutsideMargin = 10.0f;
	static constexpr float SpawnBandDepth = 40.0f;

	// 월드 가장자리 바로 위에서 생성되는 것을 방지
	static constexpr float WorldPadding = 10.0f;
};