#pragma once

#include <memory>

namespace Craft
{
	class Actor;
}

// Damage 종류
enum class DamageType
{
	None,
	Melee,			// 적 근접 공격
	Projectile,		// 총알 / 투사체
	Electric,		// 전기
	Explosion,		// 폭발
	Environment		// 맵 / 환경 피해
};

// 한 번의 데미지에 필요한 정보
// 단순히 int damage만 전달하지 않고 "누가 / 어떤 방식으로 / 얼마만큼" 공격했는지를 하나로 묶음
// ============================================================
struct DamageInfo
{
	// 실제 피해량
	int damage = 0;

	// 피해 종류
	DamageType damageType = DamageType::None;

	// Damage를 발생시킨 Actor
	// shared_ptr로 소유하면 생명주기가 꼬일 수 있으므로 weak_ptr 사용
	// ------------------------------------------------------------
	std::weak_ptr<Craft::Actor> damageCauser;

	// 무적 상태를 무시하는 피해인지
	// 기본 공격은 false
	// 추후 낙사/강제 처형 같은 특수 데미지에서 사용 가능
	// ------------------------------------------------------------
	bool ignoreInvincibility = false;
};