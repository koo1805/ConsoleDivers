#include "ArcThrower.h"
#include <Level/Level.h>
#include <Actor/Projectile/PelletProjectile/PelletProjectile.h>

#include <cmath>

ArcThrower::ArcThrower(const Craft::Vector2F& position)
	: WeaponBase(
		WeaponSlotType::Support,
		WeaponAmmoData{1, 1, 99, 99, 0.0f},
		position)
{ }

void ArcThrower::StartFire(const Craft::Vector2F & aimDirection)
{
	// 이미 충전 중이면 무시
	if (isCharging)
	{
		return;
	}

	// 장전 중이면 차지 불가
	if (IsReloading())
	{
		return;
	}

	// 차지 시작
	isCharging = true;

	chargeTime = 0.0f;
}

void ArcThrower::UpdateFire(float deltaTime, const Craft::Vector2F & aimDirection)
{
	if (!isCharging)
	{
		return;
	}

	// 무한 충전
	chargeTime += deltaTime;
}

void ArcThrower::ReleaseFire(const Craft::Vector2F & aimDirection)
{
	if (!isCharging)
	{
		return;
	}

	// Charge 종료
	isCharging = false;

	// 최소 충전 시간 미달 발사하지 않음
	if (chargeTime < minimumChargeTime)
	{
		chargeTime = 0.0f;

		return;
	}

	FireChargedShot(aimDirection);

	chargeTime = 0.0f;
}

// 충전 중 느려짐
float ArcThrower::GetMoveSpeedMultiplier() const
{
	if (!isCharging)
	{
		return 1.0f;
	}

	return chargeMoveMultiplier;
}

void ArcThrower::CancelFire()
{
	// 충전 상태 완전히 초기화
	isCharging = false;
	chargeTime = 0.0f;
}

void ArcThrower::FireChargedShot(const Craft::Vector2F& aimDirection)
{
	std::shared_ptr<Craft::Level> level = GetOwner();
	std::shared_ptr<Craft::Actor> owner = GetWeaponOwner();

	if (!level || !owner)
	{
		return;
	}

	const float centerAngle = std::atan2(aimDirection.y, aimDirection.x);

	constexpr float spreadAngle = 0.08f;
	const int centerIndex = pelletCount / 2;

	for (int index = 0; index < pelletCount; ++index)
	{
		const int spreadIndex = index - centerIndex;

		const float angle = centerAngle + static_cast<float>(spreadIndex) * spreadAngle;

		Craft::Vector2F direction(std::cos(angle), std::sin(angle));

		auto projectile = level->SpawnActor<PelletProjectile>(
				GetPosition(),
				direction,
				projectileSpeed,
				projectileLifeTime,
				false
			);

		projectile->SetProjectileOwner(owner);
	}
}
