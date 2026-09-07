#pragma once

#include <Math/Vector2.h>
#include <Render/Sprite/PixelSprite.h>
#include <Algorithm/AStar/Navigation/NavigationGrid.h>

#include <vector>

enum class MapRegionType
{
	PlayerSpawn,
	Mission,
	EnemyNest
};

// 맵의 사각형 영역
struct MapRect
{
	int x = 0;
	int y = 0;

	int width = 0;
	int height = 0;

	bool Contains(int targetX, int targetY) const
	{
		return targetX >= x && targetY >= y && targetX < x + width && targetY < y + height;
	}

	Craft::Vector2 GetCenter() const
	{
		return Craft::Vector2(x + width / 2, y + height / 2);
	}
};

// 게임에서 의미를 가지는 영역
struct MapRegion
{
	MapRegionType type = MapRegionType::PlayerSpawn;
	MapRect bounds;
};

class MapGenerator
{
public:
	// 청크 하나에 들어갈 PixelSprite생성
	static Craft::PixelSprite BuildChunkSprite(
		const Craft::Vector2& chunkPosition,
		const Craft::Vector2& chunkSize,
		const Craft::Vector2& worldSize,
		const std::vector<MapRegion>& regions,
		const std::vector<MapRect>& obstacles);

	// 장애물 데이터를 NavigationGrid에 적용
	static void ApplyNavigationData(NavigationGrid& navigationGrid, const Craft::Vector2& worldSize, const std::vector<MapRect>& obstacles);
};