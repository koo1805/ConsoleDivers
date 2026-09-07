#pragma once

#include <Actor/Map/MapGenerator.h>
#include <Math/Vector2.h>
#include <Math/Vector2F.h>

#include <memory>
#include <vector>
#include <cstddef>

namespace Craft
{
	class Level;
}

class MapChunk;

// 게임 전체 월드 관리
class WorldMap
{
public:
	WorldMap() = default;
	~WorldMap() = default;

public:
	// 월드 생성
	void Initialize(Craft::Level& level, NavigationGrid& navigationGrid);

	// 월드 전체 크기
	Craft::Vector2 GetWorldSize() const;

	// Player 시작 위치
	Craft::Vector2F GetPlayerSpawnPosition() const;

	// Enemy Nest 중심 위치
	Craft::Vector2F GetEnemyNestSpawnPosition(std::size_t index) const;

	// 현재 생성된 Enemy Nest 개수
	std::size_t GetEnemyNestCount() const;

	// Mission 지역 중심
	Craft::Vector2F GetMissionPosition() const;

private:
	void BuildRegions();

	void BuildObstacles();

	void BuildChunks(Craft::Level& level);

private:
	// ============================================================
	// World 설정
	// ============================================================

	static constexpr int WorldWidth = 720;
	static constexpr int WorldHeight = 640;

	// 한 청크 크기
	//
	// 720 / 120 = 6
	// 640 / 80  = 8
	//
	// 총 48개 Chunk
	static constexpr int ChunkWidth = 120;
	static constexpr int ChunkHeight = 80;

	// 기존 A*와 동일하게 10 World Cell 단위
	static constexpr int NavigationCellSize = 10;

private:
	// 게임 플레이용 영역
	std::vector<MapRegion> regions;

	// 이동 불가능한 지형
	std::vector<MapRect> obstacles;

	// 생성된 맵 청크
	std::vector<std::shared_ptr<MapChunk>> chunks;

	// 빠른 접근용 위치
	Craft::Vector2F playerSpawnPosition = Craft::Vector2F::Zero;

	Craft::Vector2F missionPosition = Craft::Vector2F::Zero;

	std::vector<Craft::Vector2F> enemyNestPositions;
};