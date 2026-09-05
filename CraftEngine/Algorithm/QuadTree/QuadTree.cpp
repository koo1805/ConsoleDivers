#include "QuadTree.h"
#include <Algorithm/QuadTree/QuadTreeNode.h>
#include <Actor/Actor.h>

namespace Craft
{
	QuadTree::QuadTree(const QuadTreeBounds& worldBounds, int maxDepth)
		: worldBounds(worldBounds), maxDepth(maxDepth)
	{
		root = std::make_unique<QuadTreeNode>(worldBounds, 0, maxDepth);
	}

	QuadTree::~QuadTree() = default;

	void QuadTree::Clear()
	{
		// 기존 데이터 제거
		if (root)
		{
			root->Clear();
		}
	}

	bool QuadTree::Insert(const std::shared_ptr<Actor>&actor)
	{
		if (!actor)
		{
			return false;
		}

		if (!actor->IsActive())
		{
			return false;
		}

		const QuadTreeBounds actorBounds = BuildActorBounds(actor);

		return Insert(actor, actorBounds);
	}

	bool QuadTree::Insert(const std::shared_ptr<Actor>& actor, const QuadTreeBounds& actorBounds)
	{
		if (!actor)
		{
			return false;
		}

		if (!actor->IsActive())
		{
			return false;
		}

		// 충돌 영역이 없는 Actor는 QuadTree에서 제외
		if (actorBounds.width <= 0.0f || actorBounds.height <= 0.0f)
		{
			return false;
		}

		if (!root)
		{
			return false;
		}

		return root->Insert(actor, actorBounds);
	}

	std::vector<std::shared_ptr<Actor>> QuadTree::Query(const QuadTreeBounds& queryBounds) const
	{
		std::vector<std::shared_ptr<Actor>> results;

		if (!root)
		{
			return results;
		}

		root->Query(queryBounds, results);

		return results;
	}

	QuadTreeBounds QuadTree::BuildActorBounds(const std::shared_ptr<Actor>& actor) const
	{
		if (!actor)
		{
			return QuadTreeBounds();
		}

		const Vector2F position = actor->GetPosition();

		return QuadTreeBounds(position.x, position.y, static_cast<float>(actor->GetWidth()), static_cast<float>(actor->GetHeight()));
	}
}