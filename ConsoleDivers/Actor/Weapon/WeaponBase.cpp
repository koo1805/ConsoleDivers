#include "WeaponBase.h"

using namespace Craft;

WeaponBase::WeaponBase(const Craft::Vector2F& position)
	: Actor(position)
{
	weaponState = WeaponState::Dropped;

	sortingOrder = 11;
}

void WeaponBase::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	// Drop -> 위치 갱신 필요 없음
	if (weaponState == WeaponState::Dropped)
	{
		return;
	}

	// Equip
	// 장착 상태라면 Owner를 가져옴
	std::shared_ptr<Craft::Actor> owner = weaponOwner.lock();

	// Owner가 사라지면 드랍상태로 바꿈
	if (!owner)
	{
		weaponState = WeaponState::Dropped;
		return;
	}

	// Owner 위치 추적
	const Craft::Vector2F ownerPosition = owner->GetPosition();

	// Todo: 플레이어 attach기능 구현
	SetPosition(Craft::Vector2F(ownerPosition.x + equipOffset.x, ownerPosition.y + equipOffset.y));
}

void WeaponBase::Fire()
{

}

void WeaponBase::Equip(const std::shared_ptr<Craft::Actor>& newOwner)
{
	// 예외 처리
	if (!newOwner)
	{
		return;
	}

	// Owner 저장
	weaponOwner = newOwner;

	// 장착 상태
	weaponState = WeaponState::Equipped;
}

void WeaponBase::Drop(const Craft::Vector2F& dropPosition)
{
	// Owner 연결 해제
	weaponOwner.reset();

	// 상태 변경
	weaponState = WeaponState::Dropped;

	// 드랍 위치
	SetPosition(dropPosition);
}
