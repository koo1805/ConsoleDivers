#include "QuadTreeNode.h"
#include <Actor/Actor.h>

namespace Craft
{
	QuadTreeNode::QuadTreeNode(const QuadTreeBounds& bounds, int depth, int maxDepth)
		: bounds(bounds), depth(depth), maxDepth(maxDepth)
	{}

	bool QuadTreeNode::Insert(const std::shared_ptr<Actor>&actor, const QuadTreeBounds & actorBounds)
	{
		// 잘못된 Actor
		if (!actor)
		{
			return false;
		}

		// 현재 Node 영역 밖에 존재하면 삽입할 수 없음
		if (!bounds.Contains(actorBounds))
		{
			return false;
		}

		// 어느 자식 영역에 들어가는지 확인
		const Region region = TestRegion(actorBounds);

		// 여러 사분면에 걸친 Actor
		// 특정 자식에게 강제로 넣지 않고 현재 부모 Node에 그대로 저장
		// 실제 Insert에 사용한 Bounds도 같이 저장
		if (region == Region::Straddling)
		{
			entries.emplace_back(QuadTreeEntry{actor, actorBounds});

			return true;
		}

		// 최대 깊이라서 더 나눌 수 없는 경우
		if (!Subdivide())
		{
			entries.emplace_back(QuadTreeEntry{ actor, actorBounds });

			return true;
		}

		// 완전히 포함되는 자식으로 이동
		switch (region)
		{
		case Region::TopLeft:
			return topLeft->Insert(actor, actorBounds);

		case Region::TopRight:
			return topRight->Insert(actor, actorBounds);

		case Region::BottomLeft:
			return bottomLeft->Insert(actor, actorBounds);

		case Region::BottomRight:
			return bottomRight->Insert(actor, actorBounds);

		default:
			break;
		}

		return false;
	}

	void QuadTreeNode::Query(const QuadTreeBounds& queryBounds, std::vector<std::shared_ptr<Actor>>& results) const
	{
		// 현재 QuadTreeNode 영역과 Query 영역이 겹치지 않는다면 이 아래의 자식 Node 역시 검사할 필요없음
		// QuadTree 최적화의 핵심 부분
		if (!bounds.Intersects(queryBounds))
		{
			return;
		}

		// 현재 Node에 저장된 Entry 검사
		// Actor의 현재 위치를 다시 계산하지 않고 Insert 당시 사용했던 Bounds를 사용
		// 따라서 CollisionSystem에서는 Swept Bounds가 유지
		for (const QuadTreeEntry& entry : entries)
		{
			std::shared_ptr<Actor> actor = entry.actor.lock();

			if (!actor)
			{
				continue;
			}

			if (!actor->IsActive())
			{
				continue;
			}

			if (!entry.bounds.Intersects(queryBounds))
			{
				continue;
			}

			results.emplace_back(actor);
		}

		// 아직 분할되지 않았다면 종료
		if (!IsDivided())
		{
			return;
		}

		// 네 자식 영역 재귀 탐색
		topLeft->Query(queryBounds, results);
		topRight->Query(queryBounds, results);
		bottomLeft->Query(queryBounds, results);
		bottomRight->Query(queryBounds, results);
	}

	void QuadTreeNode::Clear()
	{
		// Actor는 QuadTree가 소유하지 않으므로 Entry 목록만 제거
		entries.clear();

		// 매 프레임 다시 QuadTree를 구축할 것이므로 자식 Node 역시 제거
		topLeft.reset();
		topRight.reset();
		bottomLeft.reset();
		bottomRight.reset();
	}
	bool QuadTreeNode::Subdivide()
	{
		// 최대 깊이에 도달
		if (depth >= maxDepth)
		{
			return false;
		}

		// 이미 분할되어 있음
		if (IsDivided())
		{
			return true;
		}

		const float halfWidth = bounds.width * 0.5f;
		const float halfHeight = bounds.height * 0.5f;

		// 더 이상 의미 있게 분할할 수 없는 경우
		if (halfWidth <= 0.0f || halfHeight <= 0.0f)
		{
			return false;
		}

		// 왼쪽 위
		topLeft = std::make_unique<QuadTreeNode>(QuadTreeBounds(bounds.x, bounds.y, halfWidth, halfHeight), depth + 1, maxDepth);

		// 오른쪽 위
		topRight = std::make_unique<QuadTreeNode>(QuadTreeBounds(bounds.x + halfWidth, bounds.y, halfWidth, halfHeight), depth + 1, maxDepth);

		// 왼쪽 아래
		bottomLeft = std::make_unique<QuadTreeNode>(QuadTreeBounds(bounds.x, bounds.y + halfHeight, halfWidth, halfHeight), depth + 1, maxDepth);

		// 오른쪽 아래
		bottomRight = std::make_unique<QuadTreeNode>(QuadTreeBounds(bounds.x + halfWidth, bounds.y + halfHeight, halfWidth, halfHeight), depth + 1,maxDepth);

		// 자식 4개 생성에 성공
		return true;
	}

	QuadTreeNode::Region QuadTreeNode::TestRegion(const QuadTreeBounds& targetBounds) const
	{
		// 현재 노드 자체를 벗어남
		if (!bounds.Contains(targetBounds))
		{
			return Region::OutOfArea;
		}

		const float centerX = bounds.x + bounds.width * 0.5f;

		const float centerY = bounds.y + bounds.height * 0.5f;

		// 대상 영역 전체가 왼쪽에 존재
		const bool left = targetBounds.x >= bounds.x && targetBounds.GetMaxX() <= centerX;

		// 대상 영역 전체가 오른쪽에 존재
		const bool right = targetBounds.x >= centerX && targetBounds.GetMaxX() <= bounds.GetMaxX();

		// 대상 영역 전체가 위쪽에 존재
		const bool top = targetBounds.y >= bounds.y && targetBounds.GetMaxY() <= centerY;

		// 대상 영역 전체가 아래쪽에 존재
		const bool bottom = targetBounds.y >= centerY && targetBounds.GetMaxY() <= bounds.GetMaxY();

		if (top && left)
		{
			return Region::TopLeft;
		}

		if (top && right)
		{
			return Region::TopRight;
		}

		if (bottom && left)
		{
			return Region::BottomLeft;
		}

		if (bottom && right)
		{
			return Region::BottomRight;
		}

		// 어느 한 사분면에도 완전히 들어가지 않음 | 중앙 X/Y 경계에 걸쳐 있는 Actor
		return Region::Straddling;
	}
}