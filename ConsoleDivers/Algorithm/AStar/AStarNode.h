#pragma once

#include <Math/Vector2.h>

struct AStarNode
{
public:
	AStarNode() = default;
	AStarNode(const Craft::Vector2& position)
		: position(position)
	{}

public:
	// 현재 Node의 전체 탐색 비용
	// F = G + H
	int GetFCost() const { return gCost + hCost; }

public:
	// NavigationGrid 안에서의 현재 Cell 위치
	Craft::Vector2 position = Craft::Vector2::Zero;

	// 시작 Node에서 현재 Node까지 이동한 실제 비용
	int gCost = 0;

	// 현재 Node에서 목표 Node까지의 예상 비용
	int hCost = 0;

	// 최종 경로를 역추적하기 위한 부모 Grid 위치
	// 탐색 완료 후 Goal부터 부모를 계속 따라가면 최종 이동 경로를 복원할 수 있음
	Craft::Vector2 parentPosition = Craft::Vector2::Zero;

	// parentPosition이 실제로 설정되었는지를 구분
	bool hasParent = false;
};