#include "WeaponBase.h"
#include <Collision/ConsoleDiversCollisionLayer.h>

using namespace Craft;

WeaponBase::WeaponBase(WeaponSlotType slotType, const WeaponAmmoData& ammoData, const Craft::Vector2F& position)
	: Actor(position),
	slotType(slotType),
	ammoData(ammoData)
{
	weaponState = WeaponState::Dropped;

	sortingOrder = 15;

	// 파생 무기클래스들은 모두 Weapon Layer에 속함
	SetCollisionLayer(GameCollision::Weapon);

	// 생성 직후 상태는 Dropped - 바닥에 있는 Weapon은 Player와 충돌함
	SetCollisionMask(GameCollision::Mask(GameCollision::Player));
}

void WeaponBase::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	// 장전 진행
	UpdateReload(deltaTime);

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

		// Owner가 사라져도 다시 충돌할 수 있게 Mask복구
		SetCollisionMask(GameCollision::Mask(GameCollision::Player));

		return;
	}

	// 플레이어 왼손 위치 전달
	SetPosition(Craft::Vector2F(attachPosition.x + gripOffset.x, attachPosition.y + gripOffset.y));
}

void WeaponBase::Draw()
{
	// 현재 선택한 Weapon 하나만 손에 출력
	if (weaponState == WeaponState::Equipped && !isSelected)
	{
		return;
	}

	super::Draw();
}

void WeaponBase::StartFire(const Craft::Vector2F& aimDirection)
{}

void WeaponBase::UpdateFire(float deltaTime, const Craft::Vector2F & aimDirection)
{}

void WeaponBase::ReleaseFire(const Craft::Vector2F & aimDirection)
{}

// Reload
void WeaponBase::StartReload()
{
	// 이미 장전 중
	if (runtimeData.isReloading)
	{
		return;
	}

	// 현재 탄창이 이미 가득 참
	if (ammoData.currentAmmo >= ammoData.magazineCapacity)
	{
		return;
	}

	// 예비 탄창 없음
	if (ammoData.reserveMagazineCount <= 0)
	{
		return;
	}

	runtimeData.isReloading = true;
	runtimeData.reloadTimer = 0.0f;
}

void WeaponBase::UpdateReload(float deltaTime)
{
	if (!runtimeData.isReloading)
	{
		return;
	}

	runtimeData.reloadTimer += deltaTime;

	if (runtimeData.reloadTimer < ammoData.reloadDuration)
	{
		return;
	}

	ammoData.currentAmmo = ammoData.magazineCapacity;

	--ammoData.reserveMagazineCount;

	runtimeData.isReloading = false;
	runtimeData.reloadTimer = 0.0f;
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

	// Dropped상태가 아니니 충돌을 비활성화 함
	SetCollisionMask(Craft::CollisionMaskNone);
}

void WeaponBase::Drop(const Craft::Vector2F& dropPosition)
{
	// 진행 중인 무기 행동 종료
	CancelFire();

	// 장전도 취소
	runtimeData.isReloading = false;
	runtimeData.reloadTimer = 0.0f;

	// 손에서 선택 해제
	isSelected = false;

	// Owner 연결 해제
	weaponOwner.reset();

	// 상태 변경
	weaponState = WeaponState::Dropped;

	// Dropped상태이기 때문에 충돌 활성화
	SetCollisionMask(GameCollision::Mask(GameCollision::Player));

	// 드랍 위치
	SetPosition(dropPosition);
}

void WeaponBase::SetAttachPosition(const Craft::Vector2F& newAttachPosition)
{
	// 플레이어가 전달한 현재 손 위치 저장
	attachPosition = newAttachPosition;
}

void WeaponBase::SetFacingRight(bool newFacingRight)
{
	// 같은 방향이면 건너뜀
	if (isFacingRight == newFacingRight)
	{
		return;
	}

	// 방향 변경
	isFacingRight = newFacingRight;

	// 파생 클래스에 알림
	OnFacingChanged();
}

void WeaponBase::OnFacingChanged()
{
}

void WeaponBase::CancelFire()
{

}

bool WeaponBase::CanFire() const
{
	// 장전 중에는 발사 불가
	if (runtimeData.isReloading)
	{
		return false;
	}
	// 현재 탄창에 탄이 하나 이상 있어야 발사 가능
	return ammoData.currentAmmo > 0;
}

bool WeaponBase::ConsumeAmmo()
{
	// 발사 가능한 상태가 아니라면 탄약을 소비하지 않음
	if (!CanFire())
	{
		return false;
	}

	--ammoData.currentAmmo;

	return true;
}
