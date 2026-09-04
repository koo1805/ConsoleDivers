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
