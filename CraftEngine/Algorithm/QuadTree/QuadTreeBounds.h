#pragma once

namespace Craft
{
	struct QuadTreeBounds
	{
	public:
		QuadTreeBounds() = default;

		QuadTreeBounds(float x, float y, float width, float height)
			: x(x), y(y), width(width), height(height)
		{ }

	public:
		// 오른쪽 끝
		float GetMaxX() const
		{
			return x + width;
		}

		// 아래쪽 끝
		float GetMaxY() const
		{
			return y + height;
		}

		// 다른 Bounds가 현재 Bounds 안에 완전히 포함되는지 검사
		// 해당 Actor가 특정 사분면에 완전히 들어갔는지 확인
		bool Contains(const QuadTreeBounds& other) const
		{
			return other.x >= x && other.GetMaxX() <= GetMaxX() && other.y >= y && other.GetMaxY() <= GetMaxY();
		}

		// 두 영역이 서로 겹치는지 검사
		// Query에서 해당 QuadTreeNode를 탐색할 필요가 있는지 확인
		bool Intersects(const QuadTreeBounds& other) const
		{
			return other.x < GetMaxX() && other.GetMaxX() > x && other.y < GetMaxY() && other.GetMaxY() > y;
		}

	public:
		float x = 0.0f;
		float y = 0.0f;

		float width = 0.0f;
		float height = 0.0f;
	};
}