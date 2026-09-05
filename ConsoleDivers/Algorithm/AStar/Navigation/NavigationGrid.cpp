#include "NavigationGrid.h"

#include <algorithm>

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
