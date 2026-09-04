#include "NormalEnemy.h"
#include <Level/Level.h>
#include <Actor/Player/Player.h>
#include <Actor/Character/CharacterPart.h>

#include <memory>

using namespace Craft;

NormalEnemy::NormalEnemy(const Craft::Vector2F& position)
	: EnemyBase(position, EnemyStats{ CharacterStats{100, 30.0f}, 10, 5.0f, 50.0f, 1.0f })
{
	SetCharacterBounds(EnemyWidth, EnemyHeight);

	CharacterPart& body = AddPart(
		std::make_unique<CharacterPart>(
			CharacterPartType::Body,
			Craft::PixelSprite(EnemyWidth, EnemyHeight),
			Craft::Vector2(0, 0),
			0,
			false,
			false
		)
	);

	visual.Initialize(&body);
	animator.Initialize(&visual);
}

void NormalEnemy::BeginPlay()
{
	super::BeginPlay();

	// level
	std::shared_ptr<Craft::Level> level = GetOwner();

	if (!level)
	{
		return;
	}

	std::shared_ptr<Player> player = level->FindActor<Player>();

	if (!player)
	{
		return;
	}

	// target을 weak_ptr로 보관
	SetTarget(player);
}

void NormalEnemy::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	// 사망 상태
	if (GetEnemyState() != EnemyState::Dead)
	{
		UpdateBehavior(deltaTime);
	}

	UpdateVisual();
	
	UpdateAnimationState();

	animator.Tick(deltaTime);

	UpdateAttack();

	if (GetEnemyState() == EnemyState::Dead && animator.IsAnimationFinished())
	{
		Destroy();
	}
}

void NormalEnemy::OnDeath()
{
	super::OnDeath();
}

void NormalEnemy::MeleeAttack()
{
	// 목표 가져오기
	std::shared_ptr<Craft::Actor> target = GetTarget();

	if (!target)
	{
		return;
	}

	// Character인지 확인
	std::shared_ptr<Character> targetCharacter = std::dynamic_pointer_cast<Character>(target);

	if (!targetCharacter)
	{
		return;
	}

	// 데미지 적용
	targetCharacter->TakeDamage(GetEnemyStats().attackDamage);
}

void NormalEnemy::UpdateAttack()
{
	// Attack 상태가 아닐시 건너뜀
	if (GetEnemyState() != EnemyState::Attack)
	{
		return;
	}

	// 이미 이번 공격에서 데미지를 줬으면 건너뜀
	if(hasAppliedAttackDamage)
	{
		return;
	}

	// Attack 은 현재 2프레임 | 0 = 준비 | 1 = 공격
	if (animator.GetCurrentAnimationFrame() != 1)
	{
		return;
	}

	MeleeAttack();

	hasAppliedAttackDamage = true;

	StartAttackCooldown();
}

void NormalEnemy::UpdateBehavior(float deltaTime)
{
	if (GetEnemyState() == EnemyState::Attack)
	{
		// 공격 진행중 다른 애니메이션 진행 X
		if (!animator.IsAnimationFinished())
		{
			SetMoveDirection(Craft::Vector2F::Zero);

			return;
		}

		SetEnemyState(EnemyState::Idle);
	}

	// target 확인
	std::shared_ptr<Craft::Actor> target = GetTarget();

	if (!target)
	{
		// 목표가 사라지면 정지
		SetMoveDirection(Craft::Vector2F::Zero);
		SetEnemyState(EnemyState::Idle);

		return;
	}

	const float distanceSquared = GetDistanceSquaredToTarget();

	// 예외 처리
	if (distanceSquared < 0.0f)
	{
		return;
	}

	const EnemyStats& stats = GetEnemyStats();

	// sqrt 없이 제곱값만 비교 -> 연산 절약
	const float detectionRangeSquared = stats.detectionRange * stats.detectionRange;

	const float attackRangeSquared = stats.attackRange * stats.attackRange;

	// 목표가 공격 범위에 들어오면 Attack상태로 전환
	if (distanceSquared <= attackRangeSquared)
	{
		SetMoveDirection(Craft::Vector2F::Zero);

		// 쿨타임 종료
		if (CanAttack())
		{
			SetEnemyState(EnemyState::Attack);

			// 새 공격 시작
			hasAppliedAttackDamage = false;

			return;
		}

		SetEnemyState(EnemyState::Idle);

		return;
	}

	// 공격 범위 밖 | 감지 범위 안
	if (distanceSquared <= detectionRangeSquared)
	{
		SetEnemyState(EnemyState::Chase);

		// 현재 위치
		const Craft::Vector2F enemyPosition = GetPosition();

		// Player 위치
		const Craft::Vector2F targetPosition = target->GetPosition();

		// target 방향 계산
		const Craft::Vector2F direction(targetPosition.x - enemyPosition.x, targetPosition.y - enemyPosition.y);

		// 이동
		MoveInDirection(direction, deltaTime);

		return;
	}

	// 감지 거리 밖
	SetMoveDirection(Craft::Vector2F::Zero);

	SetEnemyState(EnemyState::Idle);
}

void NormalEnemy::UpdateVisual()
{
	// 객체가 바라보는 방향으로 Sprite좌우 반전
	visual.SetFacingRight(IsFacingRight());
}

void NormalEnemy::UpdateAnimationState()
{
	switch (GetEnemyState())
	{
	case EnemyState::Idle:
		animator.SetAnimationState(EnemyAnimationState::Idle);
		break;

	case EnemyState::Chase:
		animator.SetAnimationState(EnemyAnimationState::Walk);
		break;

	case EnemyState::Attack:
		animator.SetAnimationState(EnemyAnimationState::Attack);
		break;

	case EnemyState::Dead:
		animator.SetAnimationState(EnemyAnimationState::Death);
		break;
	}
}
