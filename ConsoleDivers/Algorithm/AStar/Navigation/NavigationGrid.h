#pragma once

#include <Math/Vector2.h>
#include <Math/Vector2F.h>

#include <vector>

// 월드 이동 가능 영역 정보를 Grid 형태로 관리
class NavigationGrid
{
public:
	NavigationGrid() = default;
	NavigationGrid(int width, int height, int cellSize);

public:
	// Grid 초기화
	void Initialize(int width, int height, int cellSize);

	// 이동 가능 여부
	void SetWalkable(int x, int y, bool walkable);

	bool IsWalkable(int x, int y) const;

	bool IsWalkable(const Craft::Vector2& gridPosition) const;

	// 두 World 위치 사이에 이동 불가능한 Grid Cell이 존재하는지 검사
	bool HasLineOfSight(const Craft::Vector2F& startWorldPosition, const Craft::Vector2F& endWorldPosition) const;

	// 처음 만나는 벽 직전 위치를 반환
	Craft::Vector2F GetLineEndBeforeWall(const Craft::Vector2F& startPosition, const Craft::Vector2F& direction, float maxDistance) const;

	// 좌표 변환
	// 월드 -> 그리드
	Craft::Vector2 WorldToGrid(const Craft::Vector2F& worldPosition) const;

	// Cell 중심 위치 반환
	// 그리드 -> 월드
	Craft::Vector2F GridToWorld(const Craft::Vector2& gridPosition) const;

	// 범위 검사
	bool IsValidGridPosition(int x, int y) const;

	inline bool IsValidGridPosition(const Craft::Vector2& position) const { return IsValidGridPosition(position.x, position.y); }

public:
	inline int GetWidth() const { return width; }
	inline int GetHeight() const { return height; }
	inline int GetCellSize() const { return cellSize; }

private:
	// 2차원 Grid를 1차원 배열로 변환
	int GetIndex(int x, int y) const;

private:
	int width = 0;
	int height = 0;
	int cellSize = 1;

	// true - 이동 가능
	std::vector<bool> walkableCells;
};

