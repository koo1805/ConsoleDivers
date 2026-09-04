#include "ProjectileBase.h"
#include <Actor/Character/Character.h>

#include <cmath>

ProjectileBase::ProjectileBase(
	const Craft::Vector2F& position,
	const Craft::Vector2F& direction,
	float speed,
	float lifeTime,
	bool isHoming)
	: Actor(position),
	speed(speed),
	lifeTime(lifeTime),
	isHoming(isHoming)
{
	SetDirection(direction);

	sortingOrder = 20;
}

void ProjectileBase::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	// 이동
	Move(deltaTime);

	// 수명
	UpdateLifeTime(deltaTime);
}

void ProjectileBase::OnCollision(const std::shared_ptr<Actor>& other)
{
	// 예외 처리
	if (!other)
	{
		return;
	}

	// Character 충돌 확인
	std::shared_ptr<Character> character = std::dynamic_pointer_cast<Character>(other);

	if (!character)
	{
		return;
	}

	if (character->IsDead())
	{
		return;
	}

	// 데미지
	character->TakeDamage(damage);

	// 적중 후 탄 사라짐
	Destroy();
}

void ProjectileBase::SetDirection(const Craft::Vector2F& newDirection)
{
	// 길이 계산
	const float lengthSquard = newDirection.x * newDirection.x + newDirection.y * newDirection.y;

	// 예외 처리
	if (lengthSquard <= 0.000001f)
	{
		direction = Craft::Vector2F::Zero;

		return;
	}

	const float length = std::sqrt(lengthSquard);

	// 정규화
	direction = Craft::Vector2F(newDirection.x / length, newDirection.y / length);
}

void ProjectileBase::SetProjectileOwner(const std::shared_ptr<Craft::Actor>& newOwner)
{
	// 약참조
	projectileOwner = newOwner;
}

void ProjectileBase::Move(float deltaTime)
{
	// 예외 처리
	if (direction == Craft::Vector2F::Zero)
	{
		return;
	}

	// 기본 직선 이동
	const Craft::Vector2F movement = direction * speed * deltaTime;

	SetPosition(GetPosition() + movement);
}

void ProjectileBase::UpdateLifeTime(float deltaTime)
{
	// 무한 수명 방지
	if (lifeTime <= 0.0f)
	{
		Destroy();
		return;
	}

	elapsedLifeTime += deltaTime;

	// 시간 누적
	if (elapsedLifeTime < lifeTime)
	{
		return;
	}

	// 삭제 요청
	Destroy();
}
