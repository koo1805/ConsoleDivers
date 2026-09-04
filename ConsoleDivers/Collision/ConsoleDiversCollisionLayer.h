#pragma once

#include <Physics/CollisionLayer.h>

namespace GameCollision
{
	constexpr Craft::CollisionLayer Player = Craft::CollisionLayer::Layer0;

	constexpr Craft::CollisionLayer Enemy = Craft::CollisionLayer::Layer1;

	constexpr Craft::CollisionLayer PlayerProjectile = Craft::CollisionLayer::Layer2;

	constexpr Craft::CollisionLayer EnemyProjectile = Craft::CollisionLayer::Layer3;

	constexpr Craft::CollisionLayer Weapon = Craft::CollisionLayer::Layer4;

	constexpr Craft::CollisionLayer World = Craft::CollisionLayer::Layer5;


	// Layer를 Mask로 변환하기 쉽게 사용하는 함수
	constexpr Craft::CollisionMask Mask(Craft::CollisionLayer layer)
	{
		return Craft::ToCollisionMask(layer);
	}
}