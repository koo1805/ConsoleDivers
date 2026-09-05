#pragma once

#include "QuadTreeBounds.h"

#include <memory>
#include <vector>

namespace Craft
{
	class Actor;

	class QuadTreeNode
	{
	private:
		// Actor가 어느 자식 영역에 들어가는지 판별한 결과
		enum class Region
		{
			TopLeft,
			TopRight,
			BottomLeft,
			BottomRight,

			// 두 개 이상의 영역에 걸쳐 있음
			Straddling,

			// 현재 Node 영역 자체를 벗어남
			OutOfArea
		};

		struct QuadTreeEntry
		{
			std::weak_ptr<Actor> actor;

			QuadTreeBounds bounds;
		};

	public:
		QuadTreeNode(const QuadTreeBounds& bounds, int depth, int maxDepth);

		~QuadTreeNode() = default;

	public:
		// Actor 삽입
		bool Insert(const std::shared_ptr<Actor>& actor, const QuadTreeBounds& actorBounds);

		// 지정한 영역과 겹치는 Actor 검색
		void Query(const QuadTreeBounds& queryBounds, std::vector<std::shared_ptr<Actor>>& results) const;

		// 현재 트리 내용 제거
		void Clear();

	public:
		inline const QuadTreeBounds& GetBounds() const { return bounds; }

		inline int GetDepth() const { return depth; }

		inline bool IsDivided() const { return topLeft != nullptr; }

		inline const QuadTreeNode* GetTopLeft() const { return topLeft.get(); }

		inline const QuadTreeNode* GetTopRight() const { return topRight.get(); }

		inline const QuadTreeNode* GetBottomLeft() const { return bottomLeft.get(); }

		inline const QuadTreeNode* GetBottomRight() const { return bottomRight.get(); }

	private:
		// 공간 4분할
		bool Subdivide();

		// Actor가 들어갈 영역 판정
		Region TestRegion(const QuadTreeBounds& targetBounds) const;

	private:
		// 현재 노드 깊이
		int depth = 0;

		// 최대 분할 깊이
		int maxDepth = 4;

		// 현재 노드가 담당하는 월드 영역
		QuadTreeBounds bounds;

		// Actor + 삽입 당시 Bounds
		std::vector<QuadTreeEntry> entries;

		// 4개의 자식 영역
		std::unique_ptr<QuadTreeNode> topLeft;
		std::unique_ptr<QuadTreeNode> topRight;
		std::unique_ptr<QuadTreeNode> bottomLeft;
		std::unique_ptr<QuadTreeNode> bottomRight;
	};
}
