#pragma once

#include <Actor/Character/Character.h>
#include <Actor/Enemy/Data/EnemyStats.h>

#include <memory>

// 전방 선언
class Player;

enum class EnemyState
{
	Idle,
	Chase,
	Attack,
	Dead
};

class EnemyBase : public Character
{
	TYPE_DECLARATIONS(EnemyBase, Character)

public:
	EnemyBase(const Craft::Vector2F position = Craft::Vector2F::Zero, const EnemyStats& stats = EnemyStats());
	virtual ~EnemyBase() = default;

public:
	// Getter
	// 현재 상태
	inline EnemyState GetEnemyState() const { return enemyState; }

	// 바라보는 방향 반환
	inline bool IsFacingRight() const { return isFacingRight; }

	// 이동 여부 반환
	inline bool IsMoving() const { return isMoving; }

	// Enemy 전용 스탯 반환
	inline const EnemyStats& GetEnemyStats() const{ return enemyStats; }

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float deltaTime) override;

	virtual void OnDeath() override;

	// Enemy 상태 변경
	void SetEnemyState(EnemyState newState);

	// 이동
	void MoveInDirection(const Craft::Vector2F& direction, float deltaTime);

	// 이동방향 기준 sprite 좌우반전
	void UpdateFacingDirection();

	// 이동 방향 설정
	void SetMoveDirection(const Craft::Vector2F& newDirection);

	// 현재 이동 방향 반환
	inline const Craft::Vector2F& GetMoveDirection() const { return moveDirection; }

	// 추적 대상 설정
	void SetTarget(const std::shared_ptr<Craft::Actor>& newTarget);

	// 목표 반환
	std::shared_ptr<Craft::Actor> GetTarget() const;

	// 목표와 거리 계산
	float GetDistanceSquaredToTarget() const;

	// 공격 쿨타임
	void UpdateAttackCooldown(float deltaTime);

	// 공격 가능 반환
	bool CanAttack() const;

	// 공격후 쿨타임 시작
	void StartAttackCooldown();

private:
	// Enemy 스탯
	EnemyStats enemyStats;

	// 행동 상태
	EnemyState enemyState = EnemyState::Idle;

	// 이동 확인 플래그
	bool isMoving = false;

	// true - 오른쪽
	bool isFacingRight = true;

	// 이동 방향
	Craft::Vector2F moveDirection = Craft::Vector2F::Zero;

	// 추적 대상
	std::weak_ptr<Craft::Actor> target;

	// 공격 쿨타임
	float attackCooldownTimer = 0.0f;
};

