#pragma once

#include <Actor/Actor.h>
#include <Math/Vector2.h>

namespace Craft
{
	class TestBG : public Actor
	{
		TYPE_DECLARATIONS(TestBG, Actor)

	public:
		TestBG();

		static Vector2 GetWorldSize();

	private:
		void BuildBackground();
	};
}