#pragma once

#include <cstdint>

namespace Craft
{
	// Actor가 '어떤 충돌 그룹에 속하는가'를 나타냄
	// Layer0 = player
	// Layer1 = Enemy
	// 와 같이 엔진 내부 말고 실제 게임 프로젝트에서 정의하여 사용
	//
	// 각 Layer는 하나의 비트만 사용

	// Layer0 = 1u << 0 => u는 unsigned int로 비트 시프트하겠다는 의미
	// Layer 최대 32개
	enum class CollisionLayer : uint32_t
	{
		None = 0,

		Layer0 = 1u << 0,
		Layer1 = 1u << 1,
		Layer2 = 1u << 2,
		Layer3 = 1u << 3,
		Layer4 = 1u << 4,
		Layer5 = 1u << 5,
		Layer6 = 1u << 6,
		Layer7 = 1u << 7
	};

	// CollisionMask
	// 어떤 CollisionLayer와 충돌할 것인지 저장하는 비트 마스크
	// Layer = Actor 자신의 그룹하나를 표현
	// Mask = 여러 Layer를 포함할 수 있음
	using CollisionMask = uint32_t;

	// 어떤 Layer와도 충돌하지 않는 Mask
	constexpr CollisionMask CollisionMaskNone = 0u;

	// 현재 제공하는 모든 레이어와 충돌하는 Mask
	constexpr CollisionMask CollisionMaskAll =
		static_cast<CollisionMask>(CollisionLayer::Layer0) |
		static_cast<CollisionMask>(CollisionLayer::Layer1) |
		static_cast<CollisionMask>(CollisionLayer::Layer2) |
		static_cast<CollisionMask>(CollisionLayer::Layer3) |
		static_cast<CollisionMask>(CollisionLayer::Layer4) |
		static_cast<CollisionMask>(CollisionLayer::Layer5) |
		static_cast<CollisionMask>(CollisionLayer::Layer6) |
		static_cast<CollisionMask>(CollisionLayer::Layer7);

	// CollisionLayer를 CollisionMask로 형변환
	// 비트연산을 위해 명시적으로 변환
	constexpr CollisionMask ToCollisionMask(CollisionLayer layer)
	{
		return static_cast<CollisionMask>(layer);
	}

	// Mask에 특정 Layer가 포함되어 있는지 검사
	constexpr bool HasCollisionLayer(CollisionMask mask, CollisionLayer layer)
	{
		return (mask & ToCollisionMask(layer)) != 0u;
	}
}