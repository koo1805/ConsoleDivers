#pragma once

#include <Algorithm/AStar/AStarNode.h>
#include <Algorithm/AStar/Navigation/NavigationGrid.h>

#include <vector>

// NavigationGrid를 이용해서 실제 A* 경로 탐색을 담당하는 클래스
class AStarPathFinder
{
private:
	// 하나의 이동 방향과 이동 비용
	struct Direction
	{
		Direction() = default;

		Direction(int x, int y, int cost)
			: x(x), y(y), cost(cost)
		{}

		int x = 0;
		int y = 0;

		// 직선 이동 = 10 | 대각선 이동 = 14
		int cost = 0;
	};

public:
	AStarPathFinder() = default;

public:
	// Grid 좌표 기준으로 경로 탐색
	std::vector<Craft::Vector2> FindPath(
		const NavigationGrid& navigationGrid,
		const Craft::Vector2& startPosition,
		const Craft::Vector2& goalPosition);

	// Debug용 데이터
	inline const std::vector<Craft::Vector2>& GetOpenList() const { return openList; }
	inline const std::vector<Craft::Vector2>& GetClosedList() const { return closedList; }
	inline const std::vector<Craft::Vector2>& GetLastPath() const { return lastPath; }

private:
	// 이전 탐색 정보 초기화
	void Clear();

	// 탐색에 사용할 Node 배열 생성
	void BuildNodes(const NavigationGrid& navigationGrid);

	// Grid 좌표에 해당하는 Node 반환
	AStarNode& GetNode(const Craft::Vector2& position);
	const AStarNode& GetNode(const Craft::Vector2& position) const;

	// 비용 계산
	// 현재 위치 -> 목표 위치의 예상 비용
	int CalculateHeuristic(const Craft::Vector2& current, const Craft::Vector2& goal) const;

	// List 검사
	bool IsInOpenList(const Craft::Vector2& position) const;
	bool IsInClosedList(const Craft::Vector2& position) const;

	// 이동 검사
	// 대각선 이동시 벽 모서리를 뚫고 지나가는지 검사
	bool IsDiagonalBlocked(
		const Craft::Vector2& current,
		const Direction& direction,
		const NavigationGrid& navigationGrid) const;

	// 경로 생성
	std::vector<Craft::Vector2> ConstructPath(const Craft::Vector2& goalPosition) const;

private:
	// 현재 검색에 사용되는 Grid 크기
	int gridWidth = 0;
	int gridHeight = 0;

	// Grid의 각 Cell에 대응하는 AStarNode
	std::vector<AStarNode> nodes;

	// 아직 탐색 예정인 Node
	std::vector<Craft::Vector2> openList;

	// 탐색 완료 Node
	std::vector<Craft::Vector2> closedList;

	// 가장 최근 탐색 결과
	std::vector<Craft::Vector2> lastPath;

	static constexpr int StraightCost = 10;
	static constexpr int DiagonalCost = 14;
};

