#pragma once

#include <Algorithm/QuadTree/QuadTreeBounds.h>

#include <memory>

namespace Craft
{
	class Actor;

	// QuadTree Query 중 발생하는 Step 종류
	enum class QuadTreeQueryStepType
	{
		// Query 영역과 겹쳐 실제로 탐색한 Node
		VisitNode,

		// Query 영역과 겹치지 않아 가지치기된 Node
		RejectNode,

		// 해당 Node에서 Query에 포함되는 Actor 발견
		FoundActor
	};

	// Query 과정 한 단계
	struct QuadTreeQueryStep
	{
		QuadTreeQueryStepType type = QuadTreeQueryStepType::VisitNode;

		// 해당 Step이 발생한 Node 영역
		QuadTreeBounds nodeBounds;

		// Node 깊이
		int depth = 0;

		// FoundActor인 경우에만 사용
		std::weak_ptr<Actor> actor;
	};
}