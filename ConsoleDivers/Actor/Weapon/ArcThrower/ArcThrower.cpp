#include "ArcThrower.h"
#include <Level/Level.h>
#include <Actor/Enemy/EnemyBase.h>
#include <Actor/Character/DamageInfo.h>
#include <Algorithm/QuadTree/QuadTree.h>
#include <Algorithm/QuadTree/QuadTreeBounds.h>
#include <Algorithm/AStar/Navigation/NavigationGrid.h>
#include <Debugging/DebugManager.h>

#include <algorithm>
#include <cmath>

using namespace Craft;
namespace
{
	Craft::Cell MakePixel(const Craft::ColorRGB& color)
	{
		Craft::Cell cell;

		cell.character = ' ';
		cell.foreground = color;
		cell.background = color;

		return cell;
	}
}

ArcThrower::ArcThrower(const Craft::Vector2F& position)
	: WeaponBase(
		WeaponSlotType::Support,
		WeaponAmmoData{ WeaponAmmoType::Infinite, 0, 0, 0, 0, 0.0f },
		WeaponCombatData{ 12 },
		position)
{
	// 오른쪽 방향 Sprite를 원본으로 한 번만 생성
	rightPixel = CreateArcThrowerSprite();

	// 왼쪽은 원본을 좌우 반전
	leftPixel = FlipHorizontal(rightPixel);

	// 기본 방향은 오른쪽
	ChangePixelSprite(rightPixel);

	// Player 손에 장착될 때 위치 보정
	gripOffset = Craft::Vector2F(-2.0f, -2.0f);
}

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

	// 실제 발사 전에 탄약을 한 발 소비
	// 탄약이 없거나 장전 중이면 ConsumeAmmo()가 false를 반환
	if (!ConsumeAmmo())
	{
		chargeTime = 0.0f;
		return;
	}

	FireChainLightning(aimDirection);

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

void ArcThrower::OnFacingChanged()
{
	if (IsFacingRight())
	{
		ChangePixelSprite(rightPixel);

		gripOffset = Craft::Vector2F(-2.0f, -2.0f);

		return;
	}

	ChangePixelSprite(leftPixel);

	// Sprite 폭에 맞춰 왼쪽 방향일 때 위치 보정
	gripOffset = Craft::Vector2F(-8.0f, -2.0f);
}

void ArcThrower::FireChainLightning(const Craft::Vector2F& aimDirection)
{
	if (!quadTree)
	{
		return;
	}

	const std::shared_ptr<Craft::Actor> weaponOwner = GetWeaponOwner();

	if (!weaponOwner)
	{
		return;
	}

	// 이번 Arc 발사에 발생하는 모든 Query 기록 시작
	Craft::DebugManager& debugManager = Craft::DebugManager::Get();

	// Arc 전용 디버그 모드일 경우 이번 한 발에서 발생하는 모든 QuadTree Query 기록 시작
	if (debugManager.IsArcThrowerDebugEnabled())
	{
		debugManager.BeginArcThrowerDebugCapture();
	}

	// 첫 번째 Target 탐색
	std::shared_ptr<EnemyBase> currentTarget = FindInitialTarget(GetPosition(), aimDirection);

	if (!currentTarget)
	{
		if (debugManager.IsArcThrowerDebugEnabled())
		{
			debugManager.EndArcThrowerDebugCapture();
		}

		return;
	}

	// 한 번의 Chain에서 이미 맞은 Enemy 기록
	std::vector<std::shared_ptr<EnemyBase>> hitTargets;

	hitTargets.reserve(maxChainTargets);

	// Chain Lightning
	for (int chainIndex = 0; chainIndex < maxChainTargets; ++chainIndex)
	{
		if (!currentTarget)
		{
			break;
		}

		ApplyElectricDamage(currentTarget);

		hitTargets.emplace_back(currentTarget);

		currentTarget = FindNextChainTarget(currentTarget, hitTargets);
	}

	// 이번 발사의 모든 Query 수집 완료
	// ============================================================
	if (debugManager.IsArcThrowerDebugEnabled())
	{
		debugManager.EndArcThrowerDebugCapture();
	}
}

std::shared_ptr<EnemyBase> ArcThrower::FindInitialTarget(const Craft::Vector2F& origin, const Craft::Vector2F& aimDirection) const
{
	if (!quadTree)
	{
		return nullptr;
	}

	// Aim Direction 정규화
	const float aimLengthSquared = aimDirection.x * aimDirection.x + aimDirection.y * aimDirection.y;

	if (aimLengthSquared <= 0.0001f)
	{
		return nullptr;
	}

	const float aimLength = std::sqrt(aimLengthSquared);

	const Craft::Vector2F normalizedAim(aimDirection.x / aimLength,aimDirection.y / aimLength);

	// QuadTree Query
	// 원형 검색 기능은 현재 없으므로 먼저 사각형으로 Broad Phase 검색 후 실제 거리 계산으로 원형 범위를 판정
	const Craft::QuadTreeBounds queryBounds(
		origin.x - initialTargetRange,
		origin.y - initialTargetRange,
		initialTargetRange * 2.0f,
		initialTargetRange * 2.0f);

	std::vector<std::shared_ptr<Craft::Actor>> candidates;

	std::vector<Craft::QuadTreeQueryStep> queryTrace;

	if (Craft::DebugManager::Get().IsArcThrowerDebugEnabled())
	{
		candidates = quadTree->QueryWithTrace(queryBounds, queryTrace);
	}
	else
	{
		candidates = quadTree->Query(queryBounds);
	}

	std::shared_ptr<EnemyBase> bestTarget;

	float bestDistanceSquared = initialTargetRange * initialTargetRange;

	for (const auto& actor : candidates)
	{
		std::shared_ptr<EnemyBase> enemy = std::dynamic_pointer_cast<EnemyBase>(actor);

		if (!enemy)
		{
			continue;
		}

		if (!enemy->IsActive())
		{
			continue;
		}

		if (enemy->IsDead())
		{
			continue;
		}

		const Craft::Vector2F enemyPosition = enemy->GetPosition();

		const float deltaX = enemyPosition.x - origin.x;

		const float deltaY = enemyPosition.y - origin.y;

		const float distanceSquared = deltaX * deltaX + deltaY * deltaY;

		// 실제 원형 Range 검사
		if (distanceSquared > initialTargetRange * initialTargetRange)
		{
			continue;
		}

		if (distanceSquared <= 0.0001f)
		{
			continue;
		}

		const float distance = std::sqrt(distanceSquared);

		const Craft::Vector2F directionToEnemy(deltaX / distance,deltaY / distance);

		// Dot Product
		// 1.0  = 정면 | 0.0  = 90도 | -1.0 = 후방
		const float aimDot = normalizedAim.x * directionToEnemy.x + normalizedAim.y * directionToEnemy.y;

		if (aimDot < minimumAimDot)
		{
			continue;
		}

		// ArcThrower는 벽을 통과해서 적을 공격할 수 없음
		// QuadTree 검색 결과에 Enemy가 포함되어 있더라도
		// Player와 Enemy 사이에 Wall이 있으면 후보에서 제외
		if (!HasLineOfSight(origin, enemyPosition))
		{
			continue;
		}

		// 전방 Cone 안에서 가장 가까운 Enemy 선택
		if (distanceSquared < bestDistanceSquared)
		{
			bestDistanceSquared = distanceSquared;

			bestTarget = enemy;
		}
	}
	// Arc Debug Mode일 때 이번 Query와 최종 선택된 Target을 DebugManager에 전달
	if (Craft::DebugManager::Get().IsArcThrowerDebugEnabled())
	{
		Craft::DebugManager::Get().AddArcThrowerQueryRecord(queryBounds, queryTrace, bestTarget);
	}

	return bestTarget;
}

std::shared_ptr<EnemyBase> ArcThrower::FindNextChainTarget(const std::shared_ptr<EnemyBase>& currentTarget, const std::vector<std::shared_ptr<EnemyBase>>& alreadyHitTargets) const
{
	if (!quadTree)
	{
		return nullptr;
	}

	if (!currentTarget)
	{
		return nullptr;
	}

	const Craft::Vector2F currentPosition = currentTarget->GetPosition();

	const Craft::QuadTreeBounds queryBounds(
		currentPosition.x - chainRange,
		currentPosition.y - chainRange,
		chainRange * 2.0f,
		chainRange * 2.0f
	);

	std::vector<std::shared_ptr<Craft::Actor>> candidates;

	std::vector<Craft::QuadTreeQueryStep> queryTrace;

	if (Craft::DebugManager::Get().IsArcThrowerDebugEnabled())
	{
		candidates = quadTree->QueryWithTrace(queryBounds, queryTrace);
	}
	else
	{
		candidates = quadTree->Query(queryBounds);
	}

	std::shared_ptr<EnemyBase> bestTarget;

	float bestDistanceSquared = chainRange * chainRange;

	for (const auto& actor : candidates)
	{
		std::shared_ptr<EnemyBase> enemy = std::dynamic_pointer_cast<EnemyBase>(actor);

		if (!enemy)
		{
			continue;
		}

		if (!enemy->IsActive())
		{
			continue;
		}

		if (enemy->IsDead())
		{
			continue;
		}

		// 이미 이번 Chain에서 맞았던 Enemy 제외
		if (HasAlreadyHit(enemy, alreadyHitTargets))
		{
			continue;
		}

		const Craft::Vector2F enemyPosition = enemy->GetPosition();

		const float deltaX = enemyPosition.x - currentPosition.x;

		const float deltaY = enemyPosition.y - currentPosition.y;

		const float distanceSquared = deltaX * deltaX + deltaY * deltaY;

		if (distanceSquared > chainRange * chainRange)
		{
			continue;
		}

		// Chain 역시 벽을 통과X
		// 현재 맞은 Enemy -> 다음 Enemy 사이에 Wall이 있으면 해당 Candidate 제외
		if (!HasLineOfSight(currentPosition, enemyPosition))
		{
			continue;
		}

		if (distanceSquared < bestDistanceSquared)
		{
			bestDistanceSquared = distanceSquared;

			bestTarget = enemy;
		}
	}

	if (Craft::DebugManager::Get().IsArcThrowerDebugEnabled())
	{
		Craft::DebugManager::Get().AddArcThrowerQueryRecord(queryBounds, queryTrace, bestTarget);
	}

	return bestTarget;
}

void ArcThrower::ApplyElectricDamage(const std::shared_ptr<EnemyBase>& target) const
{
	if (!target)
	{
		return;
	}

	if (target->IsDead())
	{
		return;
	}

	DamageInfo damageInfo;

	// WeaponCombatData에서 가져온 Damage 사용
	damageInfo.damage = GetDamage();

	damageInfo.damageType = DamageType::Electric;

	// 실제 공격자는 Weapon이 아니라 Player
	damageInfo.damageCauser = GetWeaponOwner();

	damageInfo.ignoreInvincibility = false;

	target->ApplyDamage(damageInfo);
}

bool ArcThrower::HasAlreadyHit(const std::shared_ptr<EnemyBase>& target, const std::vector<std::shared_ptr<EnemyBase>>& alreadyHitTargets) const
{
	if (!target)
	{
		return false;
	}

	for (const auto& hitTarget : alreadyHitTargets)
	{
		if (hitTarget == target)
		{
			return true;
		}
	}

	return false;
}

bool ArcThrower::HasLineOfSight(const Craft::Vector2F& startPosition, const Craft::Vector2F& endPosition) const
{
	// NavigationGrid가 없다면
	// 타격 가능 여부를 정확히 판정할 수 없으므로 차단
	if (!navigationGrid)
	{
		return false;
	}

	return navigationGrid->HasLineOfSight(startPosition, endPosition);
}

Craft::PixelSprite
ArcThrower::CreateArcThrowerSprite() const
{
	constexpr int width = 11;
	constexpr int height = 4;

	Craft::PixelSprite sprite(width, height);

	// 어두운 금속
	const Craft::ColorRGB darkMetalColor(30, 34, 36);

	// 기본 총몸
	const Craft::ColorRGB bodyColor(55, 62, 64);

	// 전기 코일 부분
	const Craft::ColorRGB electricColor(90, 190, 210);

	// 손잡이
	const Craft::ColorRGB gripColor(70, 55, 40);

	// Helldivers 계열 노란 강조
	const Craft::ColorRGB yellowColor(220, 185, 45);

	const Craft::Cell darkPixel = MakePixel(darkMetalColor);

	const Craft::Cell bodyPixel = MakePixel(bodyColor);

	const Craft::Cell electricPixel = MakePixel(electricColor);

	const Craft::Cell gripPixel = MakePixel(gripColor);

	const Craft::Cell yellowPixel = MakePixel(yellowColor);

	const char* pixels[height]
	{
		"..CCCCEEEEE",
		"GGCCCCDDDDD",
		".G.YCC.....",
		"....Y......"
	};

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			switch (pixels[y][x])
			{
			case 'C':
				sprite.SetCell(x, y, bodyPixel);
				break;

			case 'E':
				sprite.SetCell(x, y, electricPixel);
				break;

			case 'D':
				sprite.SetCell(x, y, darkPixel);
				break;

			case 'G':
				sprite.SetCell(x, y, gripPixel);
				break;

			case 'Y':
				sprite.SetCell(x, y, yellowPixel);
				break;

			case '.':
			default:
				// 투명 Cell 유지
				break;
			}
		}
	}

	return sprite;
}

Craft::PixelSprite ArcThrower::FlipHorizontal(const Craft::PixelSprite& sprite) const
{
	const int width = sprite.GetWidth();

	const int height = sprite.GetHeight();

	Craft::PixelSprite flippedSprite(width, height);

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			const int sourceX = width - 1 - x;

			flippedSprite.SetCell(x, y, sprite.GetCell(sourceX, y));
		}
	}

	return flippedSprite;
}
