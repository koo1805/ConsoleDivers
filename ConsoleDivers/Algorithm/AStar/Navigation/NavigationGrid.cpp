#include "NavigationGrid.h"

#include <algorithm>
#include <cmath>
#include <limits>

NavigationGrid::NavigationGrid(int width, int height, int cellSize)
{
    Initialize(width, height, cellSize);
}

void NavigationGrid::Initialize(int width, int height, int cellSize)
{
    this->width = (std::max)(0, width);
    this->height = (std::max)(0, height);
    this->cellSize = (std::max)(1, cellSize);

    // 기본적으로 모든 Cell 이동가능
    const int totalCellCount = this->width * this->height;

    walkableCells.assign(totalCellCount, true);
}

void NavigationGrid::SetWalkable(int x, int y, bool walkable)
{
    if (!IsValidGridPosition(x, y))
    {
        return;
    }

    const int index = GetIndex(x, y);

    walkableCells[index] = walkable;
}

bool NavigationGrid::IsWalkable(int x, int y) const
{
    if (!IsValidGridPosition(x, y))
    {
        return false;
    }

    const int index = GetIndex(x, y);
    return walkableCells[index];
}

bool NavigationGrid::IsWalkable(const Craft::Vector2& gridPosition) const
{
    return IsWalkable(gridPosition.x, gridPosition.y);
}

bool NavigationGrid::HasLineOfSight(const Craft::Vector2F& startWorldPosition, const Craft::Vector2F& endWorldPosition) const
{
	// 시작 / 끝 위치를 Grid 좌표로 변환
	Craft::Vector2 currentGrid = WorldToGrid(startWorldPosition);

	const Craft::Vector2 targetGrid = WorldToGrid(endWorldPosition);

	// Grid 밖이라면 유효한 LOS로 취급하지 않음
	if (!IsValidGridPosition(currentGrid))
	{
		return false;
	}

	if (!IsValidGridPosition(targetGrid))
	{
		return false;
	}

	// 같은 Cell이면 사이에 검사할 벽이 없음
	if (currentGrid == targetGrid)
	{
		return true;
	}

	// World 방향
	// ============================================================
	const float deltaX = endWorldPosition.x - startWorldPosition.x;
	const float deltaY = endWorldPosition.y - startWorldPosition.y;

	// X / Y 진행 방향
	const int stepX = (deltaX > 0.0f) ? 1 : (deltaX < 0.0f ? -1 : 0);
	const int stepY = (deltaY > 0.0f) ? 1 : (deltaY < 0.0f ? -1 : 0);

	// 다음 Grid 경계까지 필요한 비율 계산
	// t = 0.0 → 시작점
	// t = 1.0 → 끝점
	// ============================================================
	constexpr float infinity = std::numeric_limits<float>::infinity();

	const float cellSizeFloat = static_cast<float>(cellSize);

	const float tDeltaX = (stepX != 0) ? cellSizeFloat / std::abs(deltaX) : infinity;
	const float tDeltaY = (stepY != 0) ? cellSizeFloat / std::abs(deltaY) : infinity;

	float tMaxX = infinity;
	float tMaxY = infinity;

	if (stepX > 0)
	{
		const float nextBoundaryX = static_cast<float>((currentGrid.x + 1) * cellSize);

		tMaxX = (nextBoundaryX - startWorldPosition.x) / deltaX;
	}
	else if (stepX < 0)
	{
		const float nextBoundaryX = static_cast<float>(currentGrid.x * cellSize);

		tMaxX = (nextBoundaryX - startWorldPosition.x) / deltaX;
	}

	if (stepY > 0)
	{
		const float nextBoundaryY = static_cast<float>((currentGrid.y + 1) * cellSize);

		tMaxY = (nextBoundaryY - startWorldPosition.y) / deltaY;
	}
	else if (stepY < 0)
	{
		const float nextBoundaryY = static_cast<float>(currentGrid.y * cellSize);

		tMaxY = (nextBoundaryY - startWorldPosition.y) / deltaY;
	}

	// Grid Traversal
	// ============================================================
	while (currentGrid != targetGrid)
	{
		// 다음 X 경계가 먼저
		if (tMaxX < tMaxY)
		{
			currentGrid.x += stepX;

			tMaxX += tDeltaX;
		}
		// 다음 Y 경계가 먼저
		else if (tMaxY < tMaxX)
		{
			currentGrid.y += stepY;

			tMaxY += tDeltaY;
		}
		// 정확히 Grid 모서리를 통과
		else
		{
			currentGrid.x += stepX;
			currentGrid.y += stepY;

			tMaxX += tDeltaX;
			tMaxY += tDeltaY;
		}

		if (!IsValidGridPosition(currentGrid))
		{
			return false;
		}

		// Target이 서 있는 Cell까지는 허용
		// Enemy가 위치한 Cell 자체를 벽으로 처리하는 특수 구조가 생길 가능성도 있으므로
		// 중간 Wall만 차단
		// ========================================================
		if (!IsWalkable(currentGrid))
		{
			return false;
		}

		if (currentGrid == targetGrid)
		{
			break;
		}
	}

	return true;
}

Craft::Vector2 NavigationGrid::WorldToGrid(const Craft::Vector2F& worldPosition) const
{
    const int gridX = static_cast<int>(worldPosition.x / static_cast<float>(cellSize));
    const int gridY = static_cast<int>(worldPosition.y / static_cast<float>(cellSize));

    return Craft::Vector2(gridX, gridY);
}

Craft::Vector2F NavigationGrid::GridToWorld(const Craft::Vector2& gridPosition) const
{
    const float halfCellSize = static_cast<float>(cellSize) * 0.5f;

    const float worldX = static_cast<float>(gridPosition.x * cellSize) + halfCellSize;
    const float worldY = static_cast<float>(gridPosition.y * cellSize) + halfCellSize;

    return Craft::Vector2F(worldX, worldY);
}

// Grid 범위 검사
bool NavigationGrid::IsValidGridPosition(int x, int y) const
{
    return x >= 0 && y >= 0 && x < width && y < height;
}

int NavigationGrid::GetIndex(int x, int y) const
{
    return y * width + x;
}
