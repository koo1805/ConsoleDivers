#pragma once

#include <Core/Core.h>
#include <Algorithm/QuadTree/QuadTreeBounds.h>

#include <memory>
#include <vector>

namespace Craft
{
	// 전방 선언
	class Actor;
	class QuadTreeNode;

	class CRAFT_API QuadTree
	{
	public:
		QuadTree(const QuadTreeBounds& worldBounds, int maxDepth = 4);
		~QuadTree();

	public:
		// 모든 공간 데이터 제거
		void Clear();

		// Actor 삽입
		bool Insert(const std::shared_ptr<Actor>& actor);

		// 외부에서 계산한 Bounds를 사용해서 삽입
		bool Insert(const std::shared_ptr<Actor>& actor, const QuadTreeBounds& actorBounds);

		// 특정 영역과 겹치는 Actor 후보 검색
		std::vector<std::shared_ptr<Actor>> Query(const QuadTreeBounds& queryBounds) const;

	public:
		inline const QuadTreeNode* GetRoot() const { return root.get(); }

		inline const QuadTreeBounds& GetWorldBounds() const { return worldBounds; }

	private:
		// Actor의 현재 충돌 영역 계산
		QuadTreeBounds BuildActorBounds(const std::shared_ptr<Actor>& actor) const;

	private:
		QuadTreeBounds worldBounds;

		int maxDepth = 4;

		std::unique_ptr<QuadTreeNode> root;
	};
}
