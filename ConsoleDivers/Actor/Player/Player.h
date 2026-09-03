#pragma once

#include <Actor/Character/Character.h>
#include <Actor/Player/Sprite/PlayerVisual.h>
#include <Actor/Player/Animation/PlayerAnimator.h>

#include <memory>

// 전방 선언
class WeaponBase;

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

	// 무기 장착
	void EquipWeapon(const std::shared_ptr<WeaponBase>& weapon);

	// 무기 드랍
	void DropWeapon();

	// 장착중인 무기 반환
	std::shared_ptr<WeaponBase> GetEquippedWeapon() const;

	// 무기 장착 상태 확인
	bool HasWeapon() const;

	// 무기를 장착할 위치 반환
	Craft::Vector2F GetWeaponAttachPosition() const;

private:
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

	// 다이브 이동 방향
	Craft::Vector2F diveDirection = Craft::Vector2F::Zero;

	// 현재 장착된 무기 참조
	std::weak_ptr<WeaponBase> equippedWeapon;

	PlayerVisual visual;

	PlayerAnimator animator;

	// 기존 Player의 전체 크기 유지
	// Character가 여러 Sprite로 분리되더라도 = 카메라 / 월드 Bounds 계산에서는 기존 9 x 15 크기로 동작
	static constexpr int PlayerWidth = 9;
	static constexpr int PlayerHeight = 15;

	// 대각선 정규화
	static constexpr float diagonalScale = 0.7071f;

	// 다이브 이동 속도
	static constexpr float diveSpeed = 0.0f;

	// 다이브 지속 시간
	static constexpr float diveDuration = 3.0f;

	// 다이브 시작후 무적 유지 시간
	static constexpr float diveInvincibleDuration = 0.25f;

	// Pickup 가능 거리
	static constexpr float weaponPickupRange = 12.0f;
};

