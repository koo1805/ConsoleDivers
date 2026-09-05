#include "EnemyBase.h"
#include <Collision/ConsoleDiversCollisionLayer.h>

#include <cmath>

using namespace Craft;
EnemyBase::EnemyBase(const Craft::Vector2F position, const EnemyStats& stats)
	: Character(position, stats.characterStats), enemyStats(stats)
{
	sortingOrder = 12;

	SetCollisionLayer(GameCollision::Enemy);
	SetCollisionMask(
		GameCollision::Mask(GameCollision::Player)			 |
		GameCollision::Mask(GameCollision::PlayerProjectile) |
		GameCollision::Mask(GameCollision::World));
}

void EnemyBase::SetNavigationGrid(const NavigationGrid* newNavigationGrid)
{
	navigationGrid = newNavigationGrid;

	// Grid가 변경되면 기존 Path는 더 이상 신뢰할 수 없음
	ClearPath();

	pathRefindTimer = 0.0f;
	hasLastTargetGridPosition = false;
}

void EnemyBase::BeginPlay()
{
	super::BeginPlay();
}

void EnemyBase::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	// 공격 쿨타임 갱신
	UpdateAttackCooldown(deltaTime);

	switch (enemyState)
	{
	case EnemyState::Idle:
		break;

	case EnemyState::Chase:
		break;

	case EnemyState::Attack:
		break;

	case EnemyState::Dead:
		break;

	}
}

void EnemyBase::OnDeath()
{
	super::OnDeath();

	ClearPath();

	SetEnemyState(EnemyState::Dead);

	SetMoveDirection(Craft::Vector2F::Zero);

	SetCollisionMask(Craft::CollisionMaskNone);
}

void EnemyBase::SetEnemyState(EnemyState newState)
{
	// 같은 상태라면 건너뜀
	if (enemyState == newState)
	{
		return;
	}

	enemyState = newState;
}

void EnemyBase::MoveInDirection(const Craft::Vector2F& direction, float deltaTime)
{
	// 전달받은  방향벡터 복사
	Craft::Vector2F nomalizedDirection = direction;

	// 방향 벡터 길이 계산
	const float length = std::sqrt(nomalizedDirection.x * nomalizedDirection.x + nomalizedDirection.y * nomalizedDirection.y);

	// 이동 방향이 없는 경우
	if (length <= 0.0001f)
	{
		SetMoveDirection(Craft::Vector2F::Zero);
		return;
	}

	// 방향벡터 정규화
	nomalizedDirection.x /= length;
	nomalizedDirection.y /= length;

	// 실제 이동방향 저장
	SetMoveDirection(nomalizedDirection);

	const float moveSpeed = GetCharacterStats().moveSpeed;

	// 이동거리 = 방향 x 이동속도 x deltaTime
	position.x += nomalizedDirection.x * moveSpeed * deltaTime;
	position.y += nomalizedDirection.y * moveSpeed * deltaTime;

	// 위치 갱신
	SetPosition(position);
}

void EnemyBase::UpdateFacingDirection()
{
	// 수평 이동 없으면 방향 유지
	if (moveDirection.x == 0.0f)
	{
		return;
	}

	// 오른쪽
	if (moveDirection.x > 0.0f)
	{
		isFacingRight = true;
	}
	// 왼쪽
	else
	{
		isFacingRight = false;
	}
}

void EnemyBase::SetMoveDirection(const Craft::Vector2F& newDirection)
{
	moveDirection = newDirection;

	// 이동 방향이 0 이면 정지
	isMoving = moveDirection.x != 0.0f || moveDirection.y != 0.0f;

	// 이동 중일 때만 방향 갱신
	if (isMoving)
	{
		UpdateFacingDirection();
	}
}

void EnemyBase::SetTarget(const std::shared_ptr<Craft::Actor>& newTarget)
{
	// 약참조
	target = newTarget;
}

std::shared_ptr<Craft::Actor> EnemyBase::GetTarget() const
{
	// target이 살아있으면 shared_ptr반환
	// 없으면 nullptr반환
	return target.lock();
}

float EnemyBase::GetDistanceSquaredToTarget() const
{
	// 현재 목표 가져오기
	std::shared_ptr<Craft::Actor> targetActor = GetTarget();

	// 예외 처리
	if (!targetActor)
	{
		return -1.0f;
	}

	const Craft::Vector2F currentPosition = GetPosition();
	const Craft::Vector2F targetPosition = targetActor->GetPosition();

	const float deltaX = targetPosition.x - currentPosition.x;
	const float deltaY = targetPosition.y - currentPosition.y;

	// 제곱 값 반환 -> 연산 절약
	return deltaX * deltaX + deltaY * deltaY;
}

void EnemyBase::UpdateAttackCooldown(float deltaTime)
{
	// 예외 처리
	if (attackCooldownTimer <= 0.0f)
	{
		attackCooldownTimer = 0.0f;
		return;
	}

	attackCooldownTimer -= deltaTime;

	if (attackCooldownTimer < 0.0f)
	{
		attackCooldownTimer = 0.0f;
	}
}

bool EnemyBase::CanAttack() const
{
	return attackCooldownTimer <= 0.0f;
}

void EnemyBase::StartAttackCooldown()
{
	attackCooldownTimer = enemyStats.attackCooldown;
}

void EnemyBase::RefindPath()
{
	if (!navigationGrid)
	{
		return;
	}

	const std::shared_ptr<Craft::Actor> target = GetTarget();

	if (!target)
	{
		ClearPath();
		return;
	}

	// World -> Grid
	const Craft::Vector2 startGridPosition = navigationGrid->WorldToGrid(GetPosition());

	const Craft::Vector2 targetGridPosition = navigationGrid->WorldToGrid(target->GetPosition());

	// Grid 범위를 벗어나면 Path 생성 불가
	if (!navigationGrid->IsValidGridPosition(startGridPosition))
	{
		ClearPath();
		return;
	}

	if (!navigationGrid->IsValidGridPosition(targetGridPosition))
	{
		ClearPath();
		return;
	}

	// A* 탐색
	currentPath = pathFinder.FindPath(*navigationGrid, startGridPosition, targetGridPosition);

	currentPathIndex = 0;

	// Path[0]은 Enemy가 현재 위치한 Cell이므로 다음 Cell부터 추적
	if (currentPath.size() > 1)
	{
		currentPathIndex = 1;
	}

	lastTargetGridPosition = targetGridPosition;
	hasLastTargetGridPosition = true;
}

void EnemyBase::FollowPath(float deltaTime)
{
	if (!navigationGrid)
	{
		return;
	}

	if (currentPath.empty())
	{
		SetMoveDirection(Craft::Vector2F::Zero);

		return;
	}

	// 이미 도달한 WayPoint는 건너뜀
	while (currentPathIndex < currentPath.size() && HasReachedCurrentWayPoint())
	{
		++currentPathIndex;
	}

	// Path 끝까지 도착
	if (currentPathIndex >= currentPath.size())
	{
		SetMoveDirection(Craft::Vector2F::Zero);

		return;
	}

	const Craft::Vector2F wayPoint = navigationGrid->GridToWorld(currentPath[currentPathIndex]);

	const Craft::Vector2F currentPosition = GetPosition();

	const Craft::Vector2F direction(wayPoint.x - currentPosition.x, wayPoint.y - currentPosition.y);

	// 기존 EnemyBase 이동 함수를 그대로 사용
	MoveInDirection(direction, deltaTime);
}

void EnemyBase::UpdatePathFollowing(float deltaTime)
{
	const std::shared_ptr<Craft::Actor> target = GetTarget();

	if (!target)
	{
		ClearPath();

		SetMoveDirection(Craft::Vector2F::Zero);

		return;
	}

	// NavigationGrid가 없으면 A* 이동을 사용할 수 없음
	// NavigationGrid가 없으면 기존 직선 추적
	if (!navigationGrid)
	{
		const Craft::Vector2F currentPosition = GetPosition();

		const Craft::Vector2F targetPosition = target->GetPosition();

		const Craft::Vector2F direction(targetPosition.x - currentPosition.x, targetPosition.y - currentPosition.y);

		MoveInDirection(direction, deltaTime);

		return;
	}

	// A* Path 갱신
	pathRefindTimer -= deltaTime;

	const Craft::Vector2 targetGridPosition = navigationGrid->WorldToGrid(target->GetPosition());

	// Target이 다른 Grid Cell로 이동했는지 검사
	const bool targetGridChanged = !hasLastTargetGridPosition || targetGridPosition != lastTargetGridPosition;

	// Path 재탐색 조건
	if (currentPath.empty() || pathRefindTimer <= 0.0f || targetGridChanged)
	{
		RefindPath();

		pathRefindTimer = pathRefindInterval;
	}

	// 계산된 경로를 따라 이동
	FollowPath(deltaTime);
}

void EnemyBase::ClearPath()
{
	currentPath.clear();

	currentPathIndex = 0;

	// A* Open / Closed / LastPath 디버그 데이터도 제거
	pathFinder.PreviousPathClear();
}

bool EnemyBase::HasReachedCurrentWayPoint() const
{
	if (!navigationGrid)
	{
		return false;
	}

	if (currentPathIndex >= currentPath.size())
	{
		return false;
	}

	const Craft::Vector2F wayPoint = navigationGrid->GridToWorld(currentPath[currentPathIndex]);

	const Craft::Vector2F currentPosition = GetPosition();

	const float deltaX = wayPoint.x - currentPosition.x;
	const float deltaY = wayPoint.y - currentPosition.y;

	const float distanceSquared = deltaX * deltaX + deltaY * deltaY;

	constexpr float WayPointReachDistance =  2.0f;
	
	return distanceSquared <= WayPointReachDistance * WayPointReachDistance;
}
