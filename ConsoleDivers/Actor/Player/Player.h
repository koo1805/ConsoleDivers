#pragma once

#include <Actor/Character/Character.h>
#include <Actor/Player/Sprite/PlayerVisual.h>
#include <Actor/Player/Animation/PlayerAnimator.h>
#include <Actor/Weapon/Data/WeaponData.h>
#include <Stratagem/StratagemSystem.h>

#include <memory>

// 전방 선언
class WeaponBase;
class StratagemBeacon;

class Player : public Character
{
	TYPE_DECLARATIONS(Player, Character)

public:
	Player();

public:
	inline bool IsMoving() const { return isMoving; }

	// 현재 바라보는 방향
	inline bool IsFacingRight() const { return isFacingRight; }

	// 현재 다이브 중인지 확인
	inline bool IsDiving() const { return isDiving; }

	// 현재 무적인지 확인
	inline bool IsInvincible() const { return isInvincible; }

	// 현재 스태미나
	inline float GetCurrentStamina() const { return currentStamina; }

	// 최대 스태미나
	inline float GetMaxStamina() const { return GetCharacterStats().maxStamina; }

	// 현재 Dive 가능 여부
	inline bool CanDive() const { return currentStamina >= diveStaminaCost; }

	// 현재 Player의 스트라타젬 시스템 반환
	inline const StratagemSystem& GetStratagemSystem() const { return stratagemSystem; }

	// 무기 장착
	void EquipWeapon(const std::shared_ptr<WeaponBase>& weapon);

	// 무기 드랍
	void DropWeapon();

	// Player 사망 시 보유한 모든 Weapon을 Drop
	void DropAllWeaponsOnDeath();

	// 무기 슬롯 변경
	void ChangeWeaponSlot(WeaponSlotType newSlot);

	// 주무기 반환
	inline std::shared_ptr<WeaponBase> GetPrimaryWeapon() const { return primaryWeapon.lock(); }

	// 지원 무기 반환
	inline std::shared_ptr<WeaponBase> GetSupportWeapon() const { return supportWeapon.lock(); }

	// 현재 선택 중인 무기 슬롯 반환
	inline WeaponSlotType GetActiveWeaponSlot() const { return activeWeaponSlot; }

	// 장착중인 무기 반환
	std::shared_ptr<WeaponBase> GetEquippedWeapon() const;

	// 무기 장착 상태 확인
	bool HasWeapon() const;

	inline bool HasPrimaryWeapon() const { return !primaryWeapon.expired(); }

	inline bool HasSupportWeapon() const { return !supportWeapon.expired(); }

	// 무기를 장착할 위치 반환
	Craft::Vector2F GetWeaponAttachPosition() const;

private:
	virtual void BeginPlay() override;

	virtual void Tick(float deltaTime) override;

	void PlayerPartsGenerate();

	void Move(float xDirection, float yDirection, float deltaTime);

	// 현재 Player의 화면상 위치와 마우스 화면 위치를 비교해서 좌/우 방향 결정
	void UpdateFacingDirection();

	// 다이브 시작
	void StartDive(float xDirection, float yDirection);

	// 다이브중 시간 처리
	void UpdateDive(float deltaTime);

	// 다이브 종료
	void EndDive();

	// 주변에 떨어진 무기를 찾아 장착 시도
	void TryPickupWeapon();

	Craft::Vector2F GetAimDirection() const;

	// 스태미나 자동 회복 처리
	void UpdateStamina(float deltaTime);

	// 스태미나 소비
	bool ConsumeStamina(float amount);

	// 스트라타젬 입력 완료 후 마우스 방향으로 비콘 투척
	void ThrowStratagemBeacon(const Craft::Vector2F& direction);

protected:
	// Player Damage 판정
	// Dive 무적 상태라면 일반 Damage를 차단
	virtual bool CanReceiveDamage(const DamageInfo& damageInfo) const override;

	// 실제 피해를 받은 직후 호출
	virtual void OnDamaged(const DamageInfo& damageInfo) override;

	// 체력이 0이 됐을 때
	virtual void OnDeath() override;

private:
	bool isMoving = false;

	// true  : 오른쪽 | false : 왼쪽
	bool isFacingRight = true;

	// 다이브 상태 확인 변수
	bool isDiving = false;

	// 무적 상태 확인 변수
	bool isInvincible = false;

	// 다이브 진행 시간
	float diveTimer = 0.0f;

	// 무적 시간
	float invincibleTimer = 0.0f;

	// 현재 스태미나
	float currentStamina = 0.0f;

	// 마지막 스태미나 소비 후 경과 시간
	float staminaRecoveryTimer = 0.0f;

	// 스태미나 회복 대기 중인지
	bool isStaminaRecoveryDelayed = false;

	// 다이브 이동 방향
	Craft::Vector2F diveDirection = Craft::Vector2F::Zero;

	// 현재 보유 중인 주무기
	std::weak_ptr<WeaponBase> primaryWeapon;

	// 현재 보유 중인 지원무기
	std::weak_ptr<WeaponBase> supportWeapon;

	// 현재 손에 들고 있는 슬롯
	WeaponSlotType activeWeaponSlot = WeaponSlotType::Primary;

	PlayerVisual visual;

	PlayerAnimator animator;

	// Player 스트라타젬 입력 / 상태 관리
	StratagemSystem stratagemSystem;

	// 기존 Player의 전체 크기 유지
	// Character가 여러 Sprite로 분리되더라도 = 카메라 / 월드 Bounds 계산에서는 기존 9 x 15 크기로 동작
	static constexpr int PlayerWidth = 9;
	static constexpr int PlayerHeight = 15;

	// 대각선 정규화
	static constexpr float diagonalScale = 0.7071f;

	// 다이브 이동 속도
	static constexpr float diveSpeed = 100.0f;

	// 다이브 지속 시간
	static constexpr float diveDuration = 0.35f;

	// 다이브 시작후 무적 유지 시간
	static constexpr float diveInvincibleDuration = 0.25f;

	// Dive 1회당 소모량
	static constexpr float diveStaminaCost = 25.0f;

	// 초당 스태미나 회복량
	static constexpr float staminaRecoveryRate = 30.0f;

	// Dive 직후 회복 시작 전 대기 시간
	static constexpr float staminaRecoveryDelay = 0.6f;

	// Pickup 가능 거리
	static constexpr float weaponPickupRange = 12.0f;
};

