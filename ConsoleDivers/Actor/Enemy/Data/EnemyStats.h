#pragma once

#include <Actor/Character/CharacterStats.h>

// 체력 / 이동속도는 CharacterStats에 있음
struct EnemyStats
{
	// Character 공통 스탯
	CharacterStats characterStats;

	// 기본 공격력
	int attackDamage = 10;

	// 공격 가능한 거리
	float attackRange = 5.0f;

	// 감지 거리
	float detectionRange = 50.0f;

	// 공격 대기시간
	float attackCooldown = 1.0f;
};