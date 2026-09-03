#pragma once

#include <Actor/Actor.h>

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
	WeaponBase(const Craft::Vector2F& position = Craft::Vector2F::Zero);
	virtual ~WeaponBase() = default;

public:
	virtual void Tick(float deltaTime) override;

	virtual void Fire();

	// 장착
	void Equip(const std::shared_ptr<Craft::Actor>& newOwner);

	void Drop(const Craft::Vector2F& dropPosition);

	// Getter
	// 현재 장착 상태인지 확인
	inline bool IsEquipped() const { return weaponState == WeaponState::Equipped; }

	// 현재 드롭 상태인지 확인
	inline bool IsDropped() const { return weaponState == WeaponState::Dropped; }

	// 현재 무기를 들고있는 액터 반환
	inline std::shared_ptr<Craft::Actor> GetWeaponOwner() const { return weaponOwner.lock(); }

	// 무기 상태 반환
	inline WeaponState GetWeaponState() const { return weaponState; }

protected:
	// 장착 위치
	Craft::Vector2F equipOffset = Craft::Vector2F::Zero;

private:
	WeaponState weaponState = WeaponState::Dropped;

	// 현재 무기를 들고있는 액터
	std::weak_ptr<Craft::Actor> weaponOwner;
};

