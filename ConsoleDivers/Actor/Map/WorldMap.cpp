#include "WorldMap.h"
#include <Actor/Map/MapChunk.h>
#include <Level/Level.h>

#include <algorithm>

void WorldMap::Initialize(Craft::Level& level, NavigationGrid& navigationGrid)
{
	// ============================================================
	// 1. Map 데이터 생성
	// ============================================================

	BuildRegions();
	BuildObstacles();

	// ============================================================
	// 2. NavigationGrid 생성
	// ============================================================

	const Craft::Vector2 worldSize = GetWorldSize();

	const int gridWidth = worldSize.x / NavigationCellSize;

	const int gridHeight = worldSize.y / NavigationCellSize;

	navigationGrid.Initialize(gridWidth, gridHeight, NavigationCellSize);

	// 실제 Map 장애물과 NavigationGrid 동기화
	MapGenerator::ApplyNavigationData(navigationGrid, worldSize, obstacles);

	// ============================================================
	// 3. Chunk 생성
	// ============================================================

	BuildChunks(level);
}

Craft::Vector2 WorldMap::GetWorldSize() const
{
	return Craft::Vector2(WorldWidth, WorldHeight);
}

Craft::Vector2F WorldMap::GetPlayerSpawnPosition() const
{
	return playerSpawnPosition;
}

Craft::Vector2F WorldMap::GetEnemyNestSpawnPosition(std::size_t index) const
{
	if (index >= enemyNestPositions.size())
	{
		return Craft::Vector2F::Zero;
	}

	return enemyNestPositions[index];
}

std::size_t WorldMap::GetEnemyNestCount() const
{
	return enemyNestPositions.size();
}

Craft::Vector2F WorldMap::GetMissionPosition() const
{
	return missionPosition;
}

void WorldMap::BuildRegions()
{
	regions.clear();
	enemyNestPositions.clear();

	// ============================================================
	// Player Spawn Area
	// ============================================================

	const MapRegion playerSpawnRegion
	{
		MapRegionType::PlayerSpawn,
		MapRect
		{
			80,
			80,
			120,
			100
		}
	};

	regions.emplace_back(playerSpawnRegion);

	const Craft::Vector2 playerSpawnCenter = playerSpawnRegion.bounds.GetCenter();

	playerSpawnPosition = Craft::Vector2F(static_cast<float>(playerSpawnCenter.x), static_cast<float>(playerSpawnCenter.y));

	// ============================================================
	// Mission Area
	// ============================================================

	const MapRegion missionRegion
	{
		MapRegionType::Mission,
		MapRect
		{
			300,
			220,
			140,
			120
		}
	};

	regions.emplace_back(missionRegion);

	const Craft::Vector2 missionCenter = missionRegion.bounds.GetCenter();

	missionPosition = Craft::Vector2F(static_cast<float>(missionCenter.x), static_cast<float>(missionCenter.y));

	// ============================================================
	// Enemy Nest 1
	// ============================================================

	const MapRegion enemyNestRegion1
	{
		MapRegionType::EnemyNest,
		MapRect
		{
			520,
			100,
			100,
			100
		}
	};

	regions.emplace_back(enemyNestRegion1);

	Craft::Vector2 center = enemyNestRegion1.bounds.GetCenter();

	enemyNestPositions.emplace_back(static_cast<float>(center.x), static_cast<float>(center.y));

	// ============================================================
	// Enemy Nest 2
	// ============================================================

	const MapRegion enemyNestRegion2
	{
		MapRegionType::EnemyNest,
		MapRect
		{
			500,
			430,
			110,
			100
		}
	};

	regions.emplace_back(enemyNestRegion2);

	center = enemyNestRegion2.bounds.GetCenter();

	enemyNestPositions.emplace_back(static_cast<float>(center.x), static_cast<float>(center.y));

	// ============================================================
	// Enemy Nest 3
	// ============================================================

	const MapRegion enemyNestRegion3
	{
		MapRegionType::EnemyNest,
		MapRect
		{
			140,
			430,
			110,
			100
		}
	};

	regions.emplace_back(enemyNestRegion3);

	center = enemyNestRegion3.bounds.GetCenter();

	enemyNestPositions.emplace_back(static_cast<float>(center.x), static_cast<float>(center.y));
}

void WorldMap::BuildObstacles()
{
	obstacles.clear();

	// 세로 장애물
	obstacles.emplace_back(
		MapRect
		{
			260,
			80,
			30,
			180
		});

	// 중앙 오른쪽 가로 장애물
	obstacles.emplace_back(
		MapRect
		{
			380,
			350,
			180,
			30
		});

	// 왼쪽 하단 세로 장애물
	obstacles.emplace_back(
		MapRect
		{
			180,
			300,
			30,
			100
		});

	// 오른쪽 중앙 세로 장애물
	obstacles.emplace_back(
		MapRect
		{
			620,
			230,
			30,
			150
		});
}

void WorldMap::BuildChunks(Craft::Level& level)
{
	chunks.clear();

	const Craft::Vector2 worldSize = GetWorldSize();

	const int chunkCountX = (worldSize.x + ChunkWidth - 1) / ChunkWidth;

	const int chunkCountY = (worldSize.y + ChunkHeight - 1) / ChunkHeight;

	chunks.reserve(static_cast<std::size_t>(chunkCountX * chunkCountY));

	for (int chunkY = 0; chunkY < chunkCountY; ++chunkY)
	{
		for (int chunkX = 0; chunkX < chunkCountX; ++chunkX)
		{
			const int worldX = chunkX * ChunkWidth;

			const int worldY = chunkY * ChunkHeight;

			// 마지막 청크가 World 크기를 넘지 않도록 처리
			const int currentChunkWidth = (std::min)(ChunkWidth, worldSize.x - worldX);

			const int currentChunkHeight = (std::min)(ChunkHeight, worldSize.y - worldY);

			const Craft::Vector2 chunkPosition(worldX, worldY);

			const Craft::Vector2 chunkSize(currentChunkWidth, currentChunkHeight);

			Craft::PixelSprite chunkSprite = MapGenerator::BuildChunkSprite(chunkPosition, chunkSize, worldSize, regions, obstacles);

			std::shared_ptr<MapChunk> chunk = 
				level.SpawnActor<MapChunk>(
					Craft::Vector2F(static_cast<float>(worldX), static_cast<float>(worldY)),
					chunkSprite);

			chunks.emplace_back(chunk);
		}
	}
}