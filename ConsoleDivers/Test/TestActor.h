#pragma once

#include <Actor/Actor.h>

namespace Craft
{
	class TestActor : public Actor
	{
		TYPE_DECLARATIONS(TestActor, Actor)

	public:
		TestActor();

	public:
		inline bool IsMoving() const { return isMoving; }

	protected:
		virtual void Tick(float deltaTime) override;

		void SpriteTest();

	protected:
		bool isMoving = false;
	};
}