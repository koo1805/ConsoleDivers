#include "EnemySpawnManager.h"
#include <Level/Level.h>
#include <Render/Renderer.h>
#include <Camera/Camera.h>
#include <Util/Util.h>
#include <Algorithm/AStar/Navigation/NavigationGrid.h>

#include <Actor/Enemy/EnemyBase.h>
#include <Actor/Enemy/NormalEnemy/NormalEnemy.h>

#include <algorithm>


void EnemySpawnManager::Initialize(Craft::Level& level, NavigationGrid& navigationGrid, const Craft::Vector2& worldSize)
{
	// SpawnActor를 호출할 Level 연결
	this->level = &level;

	// 생성된 Enemy가 사용할 NavigationGrid 연결
	this->navigationGrid = &navigationGrid;

	// Spawn 위치 제한에 사용할 월드 크기 저장
	this->worldSize = worldSize;

	// Spawn 타이머 초기화
	spawnTimer = 0.0f;

	// 랜덤 Seed 초기화
	static bool hasInitializedRandomSeed = false;

	if (!hasInitializedRandomSeed)
	{
		Util::SetRandomSeed();

		hasInitializedRandomSeed = true;
	}
}

void EnemySpawnManager::Tick(float deltaTime)
{
	// 초기화가 정상적으로 되지 않았다면 실행하지 않음
	if (!level || !navigationGrid)
	{
		return;
	}

	spawnTimer += deltaTime;

	// 아직 Spawn 시간이 되지 않음
	if (spawnTimer < SpawnInterval)
	{
		return;
	}

	// Interval만큼만 감소
	spawnTimer -= SpawnInterval;

	// 현재 Enemy 수 검사
	const int aliveEnemyCount = GetAliveEnemyCount();

	// 동시에 존재 가능한 최대 Enemy 수 도달
	if (aliveEnemyCount >= MaxAliveEnemies)
	{
		return;
	}

	// 스폰
	TrySpawnEnemy();
}

int EnemySpawnManager::GetAliveEnemyCount()
{
	if (!level)
	{
		return 0;
	}

	// EnemyBase를 상속한 모든 Enemy 검색
	const std::vector<std::shared_ptr<EnemyBase>> enemies = level->FindActors<EnemyBase>();

	int aliveEnemyCount = 0;

	for (const std::shared_ptr<EnemyBase>& enemy : enemies)
	{
		if (!enemy)
		{
			continue;
		}

		// 비활성 Enemy 제외
		if (!enemy->IsActive())
		{
			continue;
		}

		// 죽은 Enemy 제외
		//
		// Death Animation이 재생 중이라
		// Actor가 아직 Level에 남아있더라도
		// Spawn 제한 개수에서는 제외한다.
		if (enemy->IsDead())
		{
			continue;
		}

		++aliveEnemyCount;
	}

	return aliveEnemyCount;
}


bool EnemySpawnManager::TrySpawnEnemy()
{
	if (!level || !navigationGrid)
	{
		return false;
	}

	// 생성 가능한 Enemy 종류가 없으면 Spawn 불가능
	if (enemyTypes.empty())
	{
		return false;
	}

	// Spawn 위치 탐색
	Craft::Vector2F spawnPosition = Craft::Vector2F::Zero;

	if (!TryFindSpawnPosition(spawnPosition))
	{
		return false;
	}

	// Enemy 종류 랜덤 선택
	const int enemyTypeIndex = Util::RandomRange(0, static_cast<int>(enemyTypes.size()) - 1);

	const EnemyType selectedEnemyType = enemyTypes[enemyTypeIndex];

	// 실제 Enemy 생성
	std::shared_ptr<EnemyBase> newEnemy = SpawnEnemy(selectedEnemyType, spawnPosition);

	if (!newEnemy)
	{
		return false;
	}

	return true;
}


std::shared_ptr<EnemyBase> EnemySpawnManager::SpawnEnemy(EnemyType enemyType, const Craft::Vector2F& spawnPosition)
{
	if (!level || !navigationGrid)
	{
		return nullptr;
	}

	std::shared_ptr<EnemyBase> newEnemy = nullptr;

	// Enemy 종류별 생성
	switch (enemyType)
	{
	case EnemyType::Normal:
	{
		std::shared_ptr<NormalEnemy> normalEnemy = level->SpawnActor<NormalEnemy>(spawnPosition);

		newEnemy = normalEnemy;

		break;
	}

	default:
		return nullptr;
	}

	// 공통 Enemy 초기화
	if (!newEnemy)
	{
		return nullptr;
	}

	// EnemyBase의 A* 기능이 사용할 NavigationGrid 연결
	newEnemy->SetNavigationGrid(navigationGrid);

	return newEnemy;
}


bool EnemySpawnManager::TryFindSpawnPosition(Craft::Vector2F& outSpawnPosition)
{
	// Renderer가 가지고 있는 실제 Camera 사용
	Craft::Camera& camera = Craft::Renderer::Get().GetCamera();

	const Craft::Vector2F cameraPosition = camera.GetPosition();

	const Craft::Vector2 viewportSize = camera.GetViewportSize();

	// Spawn 가능한 방향 조사
	std::array<SpawnSide, 4> availableSpawnSides;

	int availableSideCount = 0;


	const float worldRight = static_cast<float>(worldSize.x) - WorldPadding;

	const float worldBottom = static_cast<float>(worldSize.y) - WorldPadding;

	// Camera 왼쪽에 Spawn할 공간이 있는지 검사
	if (cameraPosition.x - SpawnOutsideMargin > WorldPadding)
	{
		availableSpawnSides[availableSideCount] = SpawnSide::Left;

		++availableSideCount;
	}

	// Right
	const float viewportRight = cameraPosition.x + static_cast<float>(viewportSize.x);

	if (viewportRight + SpawnOutsideMargin < worldRight)
	{
		availableSpawnSides[availableSideCount] = SpawnSide::Right;

		++availableSideCount;
	}

	// Top
	if (cameraPosition.y - SpawnOutsideMargin > WorldPadding)
	{
		availableSpawnSides[availableSideCount] = SpawnSide::Top;

		++availableSideCount;
	}

	// Bottom
	const float viewportBottom = cameraPosition.y + static_cast<float>(viewportSize.y);

	if (viewportBottom + SpawnOutsideMargin < worldBottom)
	{
		availableSpawnSides[availableSideCount] = SpawnSide::Bottom;

		++availableSideCount;
	}

	// 월드가 너무 작거나 Viewport가 월드를 전부 덮고 있다면 Spawn 불가능
	if (availableSideCount == 0)
	{
		return false;
	}

	// 유효한 Spawn 위치 탐색
	for (int attempt = 0; attempt < MaxSpawnPositionAttempts; ++attempt)
	{
		// 가능한 방향 중 랜덤 선택
		const int sideIndex = Util::RandomRange(0, availableSideCount - 1);

		const SpawnSide selectedSide = availableSpawnSides[sideIndex];

		// 해당 방향의 랜덤 위치 생성
		const Craft::Vector2F candidatePosition = MakeRandomSpawnPosition(selectedSide);

		// 최종 위치 검증
		if (!IsSpawnPositionValid(candidatePosition))
		{
			continue;
		}

		// 유효한 위치 발견
		outSpawnPosition = candidatePosition;

		return true;
	}

	// 최대 시도 횟수 동안 적절한 Spawn 위치를 찾지 못함
	return false;
}


Craft::Vector2F EnemySpawnManager::MakeRandomSpawnPosition(SpawnSide spawnSide) const
{
	Craft::Camera& camera = Craft::Renderer::Get().GetCamera();

	const Craft::Vector2F cameraPosition = camera.GetPosition();

	const Craft::Vector2 viewportSize = camera.GetViewportSize();

	const float worldRight = static_cast<float>(worldSize.x) - WorldPadding;

	const float worldBottom = static_cast<float>(worldSize.y) - WorldPadding;

	const float viewportRight = cameraPosition.x + static_cast<float>(viewportSize.x);

	const float viewportBottom = cameraPosition.y + static_cast<float>(viewportSize.y);

	// 기본값
	Craft::Vector2F spawnPosition = Craft::Vector2F::Zero;

	switch (spawnSide)
	{
	case SpawnSide::Left:
	{
		// Viewport 왼쪽에서 Margin만큼 더 떨어진 위치
		const float maxX = cameraPosition.x - SpawnOutsideMargin;

		// 해당 위치에서 SpawnBandDepth만큼 더 왼쪽까지 허용
		const float minX = (std::max)(WorldPadding, maxX - SpawnBandDepth);
		const float minY = (std::max)(WorldPadding, cameraPosition.y);

		const float maxY = (std::min)(worldBottom, viewportBottom);

		spawnPosition.x = Util::RandomRange(minX, maxX);

		spawnPosition.y = Util::RandomRange(minY, maxY);

		break;
	}

	case SpawnSide::Right:
	{
		const float minX = viewportRight + SpawnOutsideMargin;

		const float maxX = (std::min)(worldRight, minX + SpawnBandDepth);

		const float minY = (std::max)(WorldPadding, cameraPosition.y);

		const float maxY = (std::min)(worldBottom, viewportBottom);

		spawnPosition.x = Util::RandomRange(minX, maxX);

		spawnPosition.y =Util::RandomRange(minY, maxY);

		break;
	}

	case SpawnSide::Top:
	{
		const float maxY = cameraPosition.y - SpawnOutsideMargin;

		const float minY = (std::max)(WorldPadding, maxY - SpawnBandDepth);

		const float minX = (std::max)(WorldPadding, cameraPosition.x);

		const float maxX = (std::min)(worldRight, viewportRight);

		spawnPosition.x = Util::RandomRange(minX, maxX);

		spawnPosition.y = Util::RandomRange(minY, maxY);

		break;
	}

	case SpawnSide::Bottom:
	{
		const float minY = viewportBottom + SpawnOutsideMargin;

		const float maxY = (std::min)(worldBottom, minY + SpawnBandDepth);


		const float minX = (std::max)(WorldPadding, cameraPosition.x);

		const float maxX = (std::min)(worldRight, viewportRight);

		spawnPosition.x = Util::RandomRange(minX, maxX);

		spawnPosition.y = Util::RandomRange(minY, maxY);

		break;
	}
	}

	return spawnPosition;
}


bool EnemySpawnManager::IsSpawnPositionValid(const Craft::Vector2F& spawnPosition) const
{
	if (!navigationGrid)
	{
		return false;
	}

	// World 범위 검사
	const float worldRight = static_cast<float>(worldSize.x) - WorldPadding;

	const float worldBottom = static_cast<float>(worldSize.y) - WorldPadding;

	if (spawnPosition.x < WorldPadding || spawnPosition.y < WorldPadding || spawnPosition.x > worldRight || spawnPosition.y > worldBottom)
	{
		return false;
	}

	// Viewport 내부인지 최종 검사
	const Craft::Camera& camera = Craft::Renderer::Get().GetCamera();

	const Craft::Vector2F cameraPosition = camera.GetPosition();

	const Craft::Vector2 viewportSize = camera.GetViewportSize();

	const float viewportLeft = cameraPosition.x;

	const float viewportTop = cameraPosition.y;

	const float viewportRight = cameraPosition.x + static_cast<float>(viewportSize.x);

	const float viewportBottom = cameraPosition.y + static_cast<float>(viewportSize.y);

	const bool isInsideViewport =
		spawnPosition.x >= viewportLeft &&
		spawnPosition.x < viewportRight &&
		spawnPosition.y >= viewportTop &&
		spawnPosition.y < viewportBottom;

	// 화면 안쪽이면 Spawn 금지
	if (isInsideViewport)
	{
		return false;
	}

	// NavigationGrid 검사
	const Craft::Vector2 gridPosition = navigationGrid->WorldToGrid(spawnPosition);

	// Grid 범위 밖
	if (!navigationGrid->IsValidGridPosition(gridPosition))
	{
		return false;
	}

	// 장애물 / 이동 불가능 Cell
	if (!navigationGrid->IsWalkable(gridPosition))
	{
		return false;
	}

	return true;
}