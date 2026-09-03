#pragma once

#include <Actor/Actor.h>
#include <Actor/Weapon/Data/WeaponData.h>

#include <memory>

// 무기 현재 상태
enum class WeaponState
{
	Dropped,
	Equipped
};

class WeaponBase : public Craft::Actor
{
	TYPE_DECLARATIONS(WeaponBase, Actor)

public:
	WeaponBase(WeaponSlotType slotType, const WeaponAmmoData& ammoData, const Craft::Vector2F& position = Craft::Vector2F::Zero);
	virtual ~WeaponBase() override = default;

public:
	virtual void Tick(float deltaTime) override;

	virtual void Draw() override;

	// Fire Input
	// 마우스를 처음 눌렀을 때
	virtual void StartFire(const Craft::Vector2F& aimDirection);

	// 마우스를 누르고 있는 동안
	virtual void UpdateFire(float deltaTime, const Craft::Vector2F& aimDirection);

	// 마우스를 놓았을 때
	virtual void ReleaseFire(const Craft::Vector2F& aimDirection);

	virtual float GetMoveSpeedMultiplier() const { return 1.0f; }

	// 무기 사용 중 상태 강제 취소
	virtual void CancelFire();

	//재장전
	void StartReload();

	// 장착
	void Equip(const std::shared_ptr<Craft::Actor>& newOwner);

	void Drop(const Craft::Vector2F& dropPosition);

	// 장착할 위치 전달
	void SetAttachPosition(const Craft::Vector2F& newAttachPosition);

	void SetFacingRight(bool newFacingRight);

	void SetSelected(bool selected) { isSelected = selected; }

	inline bool IsSelected() const { return isSelected; }

	// Getter
	// 현재 장착 상태인지 확인
	inline bool IsEquipped() const { return weaponState == WeaponState::Equipped; }

	// 현재 드롭 상태인지 확인
	inline bool IsDropped() const { return weaponState == WeaponState::Dropped; }

	// 현재 무기를 들고있는 액터 반환
	inline std::shared_ptr<Craft::Actor> GetWeaponOwner() const { return weaponOwner.lock(); }

	// 무기 상태 반환
	inline WeaponState GetWeaponState() const { return weaponState; }

	// 무기가 바라볼 방향
	inline bool IsFacingRight() const { return isFacingRight; }

	// 재장전
	inline bool IsReloading() const { return runtimeData.isReloading; }

	inline WeaponSlotType GetSlotType() const { return slotType; }

	// 현재 탄약
	inline int GetCurrentAmmo() const { return ammoData.currentAmmo; }

	// 탄창 용량
	inline int GetMagazineCapacity() const { return ammoData.magazineCapacity; }

	// 탄창 개수
	inline int GetReserveMagazineCount() const { return ammoData.reserveMagazineCount; }

private:
	void UpdateReload(float deltaTime);

protected:
	virtual void OnFacingChanged();

	// 실제 탄 소비 가능 여부
	bool CanFire() const;

	// 한 발 소비
	bool ConsumeAmmo();

protected:
	// 장착 위치
	Craft::Vector2F attachPosition = Craft::Vector2F::Zero;

	// 장착 위치 보정
	Craft::Vector2F gripOffset = Craft::Vector2F::Zero;

private:
	WeaponSlotType slotType = WeaponSlotType::Primary;

	WeaponState weaponState = WeaponState::Dropped;

	WeaponAmmoData ammoData;

	WeaponRuntimeData runtimeData;

	// 현재 무기를 들고있는 액터
	std::weak_ptr<Craft::Actor> weaponOwner;

	// 현재 손에 들고 있는 Weapon인지
	bool isSelected = false;

	// true = 오른쪽
	bool isFacingRight = true;
};