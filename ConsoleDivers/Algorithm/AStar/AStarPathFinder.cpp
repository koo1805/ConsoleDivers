#include "AStarPathFinder.h"

#include <algorithm>



std::vector<Craft::Vector2> AStarPathFinder::FindPath(const NavigationGrid& navigationGrid, const Craft::Vector2& startPosition, const Craft::Vector2& goalPosition)
{
    // 이전 탐색 결과 초기화
    Clear();

    // 1. 시작 / 목표 위치 검사
    if (!navigationGrid.IsValidGridPosition(startPosition))
    {
        return {};
    }

    if (!navigationGrid.IsValidGridPosition(goalPosition))
    {
        return {};
    }

    // 시작점 또는 목표점이 이동 불가능하면 탐색할 수 없음
    if (!navigationGrid.IsWalkable(startPosition) || !navigationGrid.IsWalkable(goalPosition))
    {
        return {};
    }

    // 2. A* Node 배열 준비
    BuildNodes(navigationGrid);

    // 시작 Node
    AStarNode& startNode = GetNode(startPosition);

    startNode.gCost = 0;
    startNode.hCost = CalculateHeuristic(startPosition, goalPosition);

    startNode.hasParent = false;

    // 탐색 시작
    openList.emplace_back(startPosition);

    // 3. 이동 가능한 8방향
    const Direction directions[] =
    {
        // 상 / 하
        { 0, -1, StraightCost },
        { 0,  1, StraightCost },

        // 좌 / 우
        { -1, 0, StraightCost },
        {  1, 0, StraightCost },

        // 좌상 / 우상
        { -1, -1, DiagonalCost },
        {  1, -1, DiagonalCost },

        // 좌하 / 우하
        { -1,  1, DiagonalCost },
        {  1,  1, DiagonalCost }
    };

    // 4. Open List가 빌 때까지 탐색
    while (!openList.empty())
    {
        // F Cost가 가장 작은 Node 선택
        int bestIndex = 0;

        for (int index = 1; index < static_cast<int>(openList.size()); ++index)
        {
            const AStarNode& candidate = GetNode(openList[index]);

            const AStarNode& currentBest = GetNode(openList[bestIndex]);

            // F가 더 작은 Node를 우선 선택
            // F가 같으면 H가 더 작은 Node 선택
            if (candidate.GetFCost() < currentBest.GetFCost() || (candidate.GetFCost() == currentBest.GetFCost() && candidate.hCost < currentBest.hCost))
            {
                bestIndex = index;
            }
        }

        // 현재 탐색할 Grid 좌표
        const Craft::Vector2 currentPosition = openList[bestIndex];

        AStarNode& currentNode = GetNode(currentPosition);

        // 목표 도착 확인
        if (currentPosition == goalPosition)
        {
            lastPath = ConstructPath(goalPosition);

            return lastPath;
        }

        // Open -> Closed
        openList.erase(openList.begin() + bestIndex);

        closedList.emplace_back(currentPosition);

        // 주변 8방향 검사
        for (const Direction& direction : directions)
        {
            const Craft::Vector2 neighborPosition(currentPosition.x + direction.x, currentPosition.y + direction.y);

            // Grid 범위 밖
            if (!navigationGrid.IsValidGridPosition(neighborPosition))
            {
                continue;
            }

            // 이동 불가능한 Cell
            if (!navigationGrid.IsWalkable(neighborPosition))
            {
                continue;
            }

            // 대각선 이동시 벽 모서리 통과 방지
            if (IsDiagonalBlocked(currentPosition, direction, navigationGrid))
            {
                continue;
            }

            // 이미 탐색이 끝난 Node는 다시 Open List에 넣지 않음
            if (IsInClosedList(neighborPosition))
            {
                continue;
            }

            // 현재 Node를 거쳐 Neighbor로 갈 때의 비용
            const int newGCost = currentNode.gCost + direction.cost;

            const bool alreadyOpen = IsInOpenList(neighborPosition);

            AStarNode& neighborNode = GetNode(neighborPosition);

            // 처음 발견한 Node이거나 기존보다 더 저렴한 경로를 발견한 경우 갱신
            if (!alreadyOpen || newGCost < neighborNode.gCost)
            {
                neighborNode.gCost = newGCost;

                neighborNode.hCost = CalculateHeuristic(neighborPosition, goalPosition);

                neighborNode.parentPosition = currentPosition;

                neighborNode.hasParent = true;

                // 처음 발견한 Node라면 Open List에 추가
                if (!alreadyOpen)
                {
                    openList.emplace_back(neighborPosition);
                }
            }
        }
    }

    // 경로를 찾지 못함
    return {};
}

void AStarPathFinder::Clear()
{
    nodes.clear();

    openList.clear();
    closedList.clear();

    lastPath.clear();

    gridWidth = 0;
    gridHeight = 0;
}

void AStarPathFinder::BuildNodes(const NavigationGrid & navigationGrid)
{
    gridWidth = navigationGrid.GetWidth();
    gridHeight = navigationGrid.GetHeight();

    nodes.clear();

    nodes.reserve(gridWidth * gridHeight);

    // NavigationGrid의 모든 Cell에 대응되는 AStarNode를 하나씩 생성
    for (int y = 0; y < gridHeight; ++y)
    {
        for (int x = 0; x < gridWidth; ++x)
        {
            nodes.emplace_back(Craft::Vector2(x, y));
        }
    }
}

AStarNode& AStarPathFinder::GetNode(const Craft::Vector2 & position)
{
    const int index = position.y * gridWidth + position.x;

    return nodes[index];
}

const AStarNode& AStarPathFinder::GetNode(const Craft::Vector2& position) const
{
    const int index = position.y * gridWidth + position.x;

    return nodes[index];
}

int AStarPathFinder::CalculateHeuristic(const Craft::Vector2& current, const Craft::Vector2& goal) const
{
    const int diffX = std::abs(current.x - goal.x);
    const int diffY = std::abs(current.y - goal.y);

    // 대각선으로 이동할 수 있는 거리
    const int diagonalDistance = (std::min)(diffX, diffY);

    // 대각선 이동 이후 남는 직선 거리
    const int straightDistance = (std::max)(diffX, diffY) - diagonalDistance;

    return diagonalDistance * DiagonalCost + straightDistance * StraightCost;
}

bool AStarPathFinder::IsInOpenList(const Craft::Vector2& position) const
{
    for (const Craft::Vector2& openPosition : openList)
    {
        if (openPosition == position)
        {
            return true;
        }
    }

    return false;
}

bool AStarPathFinder::IsInClosedList(const Craft::Vector2& position) const
{
    for (const Craft::Vector2& closedPosition : closedList)
    {
        if (closedPosition == position)
        {
            return true;
        }
    }

    return false;
}

bool AStarPathFinder::IsDiagonalBlocked(const Craft::Vector2& current, const Direction& direction, const NavigationGrid& navigationGrid) const
{
    // 상하좌우 이동이면 검사 필요 없음
    if (direction.x == 0 || direction.y == 0)
    {
        return false;
    }

    // 대각선 이동 방향을 X / Y 방향으로 분리해서 검사
    const Craft::Vector2 horizontalPosition(current.x + direction.x, current.y);

    const Craft::Vector2 verticalPosition(current.x, current.y + direction.y);

    // 둘 중 하나라도 벽이면 해당 대각선 이동을 허용하지 않음
    return !navigationGrid.IsWalkable(horizontalPosition) || !navigationGrid.IsWalkable(verticalPosition);
}

std::vector<Craft::Vector2> AStarPathFinder::ConstructPath(const Craft::Vector2& goalPosition) const
{
    std::vector<Craft::Vector2> path;

    Craft::Vector2 currentPosition = goalPosition;

    // 잘못된 Parent 때문에 무한루프가 발생하지 않도록 최대 Node 수까지만 역추적함
    const int maximumPathLength = static_cast<int>(nodes.size());

    for (int count = 0; count < maximumPathLength; ++count)
    {
        path.emplace_back(currentPosition);

        const AStarNode& node = GetNode(currentPosition);

        // Start Node 도착
        if (!node.hasParent)
        {
            break;
        }

        currentPosition = node.parentPosition;
    }

    // Goal → Start 순으로 들어갔기 때문에 Start → Goal 순서로 뒤집음
    std::reverse(path.begin(), path.end());

    return path;
}
