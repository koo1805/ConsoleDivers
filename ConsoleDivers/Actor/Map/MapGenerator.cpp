#include "MapGenerator.h"

#include <Render/Cell.h>
#include <Math/ColorRGB.h>

#include <algorithm>

Craft::PixelSprite MapGenerator::BuildChunkSprite(
	const Craft::Vector2& chunkPosition,
	const Craft::Vector2& chunkSize,
	const Craft::Vector2& worldSize,
	const std::vector<MapRegion>& regions,
	const std::vector<MapRect>& obstacles)
{
	Craft::PixelSprite sprite(chunkSize.x, chunkSize.y);

	// 기본 지형 색상
	// ============================================================
	const Craft::ColorRGB floorColorA(31, 37, 32);
	const Craft::ColorRGB floorColorB(35, 41, 35);

	// Player 시작 지역
	const Craft::ColorRGB spawnColor(35, 70, 45);

	// Mission 지역
	const Craft::ColorRGB missionColor(75, 70, 35);

	// Enemy Nest 지역
	const Craft::ColorRGB enemyNestColor(75, 35, 35);

	// 장애물
	const Craft::ColorRGB obstacleColor(70, 65, 55);

	// 월드 외곽 경계
	const Craft::ColorRGB boundaryColor(50, 50, 50);

	// Navigation Cell과 동일한 10칸 두께를 월드 경계로 사용
	constexpr int boundaryThickness = 10;

	// Chunk Pixel 생성
	// ============================================================
	for (int localY = 0; localY < chunkSize.y; ++localY)
	{
		for (int localX = 0; localX < chunkSize.x; ++localX)
		{
			const int worldX = chunkPosition.x + localX;

			const int worldY = chunkPosition.y + localY;

			Craft::Cell cell;

			cell.character = ' ';

			// 기본 바닥
			// ----------------------------------------------------
			const bool alternateFloor = ((worldX / 8) + (worldY / 8)) % 2 == 0;

			const Craft::ColorRGB floorColor = alternateFloor ? floorColorA : floorColorB;

			cell.foreground = floorColor;
			cell.background = floorColor;

			// 게임 영역 표시
			// ----------------------------------------------------
			for (const MapRegion& region : regions)
			{
				if (!region.bounds.Contains(worldX, worldY))
				{
					continue;
				}

				switch (region.type)
				{
				case MapRegionType::PlayerSpawn:
					cell.foreground = spawnColor;
					cell.background = spawnColor;
					break;

				case MapRegionType::Mission:
					cell.foreground = missionColor;
					cell.background = missionColor;
					break;

				case MapRegionType::EnemyNest:
					cell.foreground = enemyNestColor;
					cell.background = enemyNestColor;
					break;
				}

				break;
			}

			// 장애물
			// ----------------------------------------------------
			for (const MapRect& obstacle : obstacles)
			{
				if (!obstacle.Contains(worldX, worldY))
				{
					continue;
				}

				cell.foreground = obstacleColor;
				cell.background = obstacleColor;

				break;
			}

			// 월드 외곽
			// NavigationGrid에서도 동일한 영역을 이동 불가 처리함
			// ----------------------------------------------------
			const bool isBoundary =
				worldX < boundaryThickness ||
				worldY < boundaryThickness ||
				worldX >= worldSize.x - boundaryThickness ||
				worldY >= worldSize.y - boundaryThickness;

			if (isBoundary)
			{
				cell.foreground = boundaryColor;
				cell.background = boundaryColor;
			}

			sprite.SetCell(
				localX,
				localY,
				cell);
		}
	}

	return sprite;
}

void MapGenerator::ApplyNavigationData(NavigationGrid& navigationGrid, const Craft::Vector2& worldSize, const std::vector<MapRect>& obstacles)
{
	const int cellSize = navigationGrid.GetCellSize();

	const int gridWidth = navigationGrid.GetWidth();

	const int gridHeight = navigationGrid.GetHeight();

	if (cellSize <= 0)
	{
		return;
	}

	// 월드 외곽 이동 금지
	for (int x = 0; x < gridWidth; ++x)
	{
		navigationGrid.SetWalkable(x, 0, false);
		navigationGrid.SetWalkable(x, gridHeight - 1, false);
	}

	for (int y = 0; y < gridHeight; y++)
	{
		navigationGrid.SetWalkable(0, y, false);
		navigationGrid.SetWalkable(gridWidth - 1, y, false);
	}

	// 장애물 적용
	for (const MapRect& obstacle : obstacles)
	{
		const int startGridX = obstacle.x / cellSize;
		const int startGridY = obstacle.y / cellSize;

		const int endGridX = (obstacle.x + obstacle.width - 1) / cellSize;
		const int endGridY = (obstacle.y + obstacle.height - 1) / cellSize;

		for (int gridY = startGridY; gridY <= endGridY; ++gridY)
		{
			for (int gridX = startGridX; gridX <= endGridX; ++gridX)
			{
				navigationGrid.SetWalkable(gridX, gridY, false);
			}
		}

	}
}