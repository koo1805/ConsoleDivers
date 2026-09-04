#include "Player.h"
#include <Level/Level.h>
#include <Render/Renderer.h>
#include <Camera/Camera.h>
#include <Input/Input.h>
#include <Collision/ConsoleDiversCollisionLayer.h>
#include <Actor/Weapon/WeaponBase.h>
#include <Actor/Weapon/Shotgun/Shotgun.h>

#include <memory>
#include <cmath>

using namespace Craft;
Player::Player()
	: Character(Craft::Vector2F(120.0f, 50.0f), CharacterStats{100, 80.0f})
{
	sortingOrder = 10;

	SetCharacterBounds(PlayerWidth, PlayerHeight);

	// 충돌 Layer 설정
	SetCollisionLayer(GameCollision::Player);
	// 충돌 Mask 설정
	SetCollisionMask(
		GameCollision::Mask(GameCollision::Enemy)			|
		GameCollision::Mask(GameCollision::EnemyProjectile)	|
		GameCollision::Mask(GameCollision::Weapon)			|
		GameCollision::Mask(GameCollision::World));

	// 부위 Sprite 생성 및 Character에 등록
	PlayerPartsGenerate();

	// Visual이 실제 Sprite / 위치 / Sorting 설정
	visual.Initialize(
		GetPart(CharacterPartType::Head),
		GetPart(CharacterPartType::Body),
		GetPart(CharacterPartType::Legs),
		GetPart(CharacterPartType::LeftHand),
		GetPart(CharacterPartType::RightHand));

	// animator 초기화
	animator.Initialized(&visual);
}

void Player::EquipWeapon(const std::shared_ptr<WeaponBase>& weapon)
{
	// 예외 처리
	if (!weapon)
	{
		return;
	}

	// 주무기
	if (weapon->GetSlotType() == WeaponSlotType::Primary)
	{
		std::shared_ptr<WeaponBase> oldWeapon = primaryWeapon.lock();

		// 같은 Weapon이면 다시 처리할 필요 없음
		if (oldWeapon == weapon)
		{
			return;
		}

		// 기존 주무기가 있다면 교체
		if (oldWeapon)
		{
			oldWeapon->Drop(GetPosition());
		}

		// 슬롯 등록
		primaryWeapon = weapon;
	}
	// 지원 무기
	else
	{
		std::shared_ptr<WeaponBase> oldWeapon = supportWeapon.lock();

		if (oldWeapon == weapon)
		{
			return;
		}

		if (oldWeapon)
		{
			// 지원 무기는 자유롭게 드롭가능
			oldWeapon->Drop(GetPosition());
		}

		// 슬롯 등록
		supportWeapon = weapon;
	}

	// Actor가 enable_shared_from_this를 가지므로 플레이어 자신을 Weapon에게 넘길수 있음
	weapon->Equip(shared_from_this());

	// 초기 방향 동기화
	weapon->SetFacingRight(isFacingRight);

	// 초기 Attach 동기화
	weapon->SetAttachPosition(GetWeaponAttachPosition());

	// 선택 상태 변경
	ChangeWeaponSlot(weapon->GetSlotType());
}

void Player::DropWeapon()
{
	// 현재 장착된 무기 확인
	std::shared_ptr<WeaponBase> weapon = GetEquippedWeapon();

	// 장착된 무기가 없다면 건너뛰기
	if (!weapon)
	{
		return;
	}

	// 주무기 수동 Drop 금지
	if (weapon->GetSlotType() == WeaponSlotType::Primary)
	{
		return;
	}

	// 드랍 위치
	const Craft::Vector2F dropPosition = GetPosition();

	// 무기 드랍
	weapon->Drop(dropPosition);

	// Player 슬롯에서도 제거
	supportWeapon.reset();

	// 지원무기를 버리면 자동으로 주무기로 전환
	ChangeWeaponSlot(WeaponSlotType::Primary);
}

void Player::ChangeWeaponSlot(WeaponSlotType newSlot)
{
	// 변경할 슬롯에 실제 Weapon이 있는지 확인
	if (newSlot == WeaponSlotType::Primary)
	{
		if (primaryWeapon.expired())
		{
			return;
		}
	}
	else
	{
		if (supportWeapon.expired())
		{
			return;
		}
	}

	// 기존 Weapon 숨김
	if (std::shared_ptr<WeaponBase> currentWeapon = GetEquippedWeapon())
	{
		// 사용 중이던 차지 / 연사 상태 취소
		currentWeapon->CancelFire();

		currentWeapon->SetSelected(false);
	}

	// 슬롯 변경
	activeWeaponSlot = newSlot;

	// 새 Weapon 활성화
	if (std::shared_ptr<WeaponBase> newWeapon = GetEquippedWeapon())
	{
		newWeapon->SetSelected(true);

		newWeapon->SetAttachPosition(GetWeaponAttachPosition());

		newWeapon->SetFacingRight(isFacingRight);
	}
}

std::shared_ptr<WeaponBase> Player::GetEquippedWeapon() const
{
	if (activeWeaponSlot == WeaponSlotType::Primary)
	{
		return primaryWeapon.lock();
	}

	return supportWeapon.lock();
}

bool Player::HasWeapon() const
{
	// weak_ptr이 만료되지 않았다면 장착한 무기가 존재한다는 것
	return !primaryWeapon.expired() || !supportWeapon.expired();
}

Craft::Vector2F Player::GetWeaponAttachPosition() const
{
	// 왼손 검색
	const CharacterPart* leftHand = GetPart(CharacterPartType::LeftHand);

	// 예외 처리
	if (!leftHand)
	{
		return GetPosition();
	}

	// 왼손 상대 위치
	const Craft::Vector2 handLocalPosition = leftHand->GetLocalPosition();

	// 로컬 위치 -> 월드 위치
	return Craft::Vector2F(position.x + static_cast<float>(handLocalPosition.x), position.y + static_cast<float>(handLocalPosition.y));
}

void Player::BeginPlay()
{
	super::BeginPlay();

	std::shared_ptr<Craft::Level> level = GetOwner();

	if (!level)
	{
		return;
	}

	// 기본 주무기 생성
	std::shared_ptr<Shotgun> shotgun = level->SpawnActor<Shotgun>(GetPosition());

	// 생성 직후 주무기 슬롯에 등록
	EquipWeapon(shotgun);
}

void Player::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	// 방향키 이동이 없으면 false
	isMoving = false;

	// 방향키 입력에 따른 이동 방향 설정
	// 오른쪽 | 아래 방향: 1 
	// 왼쪽 | 위 방향: -1
	float xDirection = 0.0f;
	float yDirection = 0.0f;

	// Left
	if (Input::Get().GetKey('A'))
	{
		xDirection = -1.0f;
		isMoving = true;
	}

	// Right
	if (Input::Get().GetKey('D'))
	{
		xDirection = 1.0f;
		isMoving = true;
	}

	// Up
	if (Input::Get().GetKey('W'))
	{
		yDirection = -1.0f;
		isMoving = true;
	}

	// Down
	if (Input::Get().GetKey('S'))
	{
		yDirection = 1.0f;
		isMoving = true;
	}

	// Dive
	if (Input::Get().GetKeyDown(VK_SPACE))
	{
		StartDive(xDirection, yDirection);
	}
	
	// 장비 집기
	if (Input::Get().GetKeyDown('E'))
	{
		TryPickupWeapon();
	}

	// 장비 드랍
	if (Input::Get().GetKeyDown('X'))
	{
		DropWeapon();
	}

	const Craft::Vector2F aimDirection = GetAimDirection();
	std::shared_ptr<WeaponBase> activeWeapon = GetEquippedWeapon();

	// 발사
	if (activeWeapon)
	{
		// 마우스를 처음 누른 순간
		if (Input::Get().GetKeyDown(VK_LBUTTON))
		{
			activeWeapon->StartFire(aimDirection);
		}

		// 누르고 있는 동안
		if (Input::Get().GetKey(VK_LBUTTON))
		{
			activeWeapon->UpdateFire(deltaTime, aimDirection);
		}

		// 마우스를 놓은 순간
		if (Input::Get().GetKeyUp(VK_LBUTTON))
		{
			activeWeapon->ReleaseFire(aimDirection);
		}

		// R : 장전
		if (Input::Get().GetKeyDown('R'))
		{
			activeWeapon->StartReload();
		}
	}

	// 1 : 주무기
	if (Input::Get().GetKeyDown('1') && activeWeaponSlot != WeaponSlotType::Primary)
	{
		ChangeWeaponSlot(WeaponSlotType::Primary);
	}

	// 지원무기
	if (Input::Get().GetKeyDown('2') && activeWeaponSlot != WeaponSlotType::Support)
	{
		ChangeWeaponSlot(WeaponSlotType::Support);
	}

	// ================================== 테스트 ===================================
	// 1 : 머리 파괴
	if (Input::Get().GetKeyDown('9'))
	{
		ApplyPartDamage(
			CharacterPartType::Head,
			9999);
	}
	// 2 : 머리 복구
	if (Input::Get().GetKeyDown('0'))
	{
		RestorePart(
			CharacterPartType::Head);
	}

	if (isDiving)
	{
		UpdateDive(deltaTime);
	}
	else
	{
		// 이동 함수 호출
		Move(xDirection, yDirection, deltaTime);
	}

	//이동이 끝난 현재 Player 위치를 기준 마우스가 좌/우 어디에 있는지 판단
	UpdateFacingDirection();

	// Player 이동 상태를 Visual Animation 상태로 전달
	if (isDiving)
	{
		animator.SetAnimationState(PlayerAnimationState::Dive);
	}
	else if (isMoving)
	{
		animator.SetAnimationState(PlayerAnimationState::Walk);
	}
	else
	{
		animator.SetAnimationState(PlayerAnimationState::Idle);
	}

	animator.Tick(deltaTime);

	if (std::shared_ptr<WeaponBase> weapon = GetEquippedWeapon())
	{
		// 손 위치 전달
		weapon->SetAttachPosition(GetWeaponAttachPosition());

		// 현재 플레이어 좌우방향 전달
		weapon->SetFacingRight(isFacingRight);
	}

	// ESC로 프로그램 종료 테스트
	if (Input::Get().GetKeyDown(VK_ESCAPE))
	{
		QuitGame();
	}
}

void Player::PlayerPartsGenerate()
{
	AddPart(
		std::make_unique<CharacterPart>(
			CharacterPartType::Legs,
			Craft::PixelSprite(),
			Craft::Vector2(0, 13),
			0,
			true,
			false),
		40);

	AddPart(
		std::make_unique<CharacterPart>(
			CharacterPartType::Body,
			Craft::PixelSprite(),
			Craft::Vector2(0, 7),
			1,
			false,
			true),
		100);

	AddPart(
		std::make_unique<CharacterPart>(
			CharacterPartType::Head,
			Craft::PixelSprite(),
			Craft::Vector2(0, 0),
			2,
			true,
			false),
		50);
	
	AddPart(
		std::make_unique<CharacterPart>(
			CharacterPartType::LeftHand,
			Craft::PixelSprite(),
			Craft::Vector2(2, 9),
			3,
			true,
			false),
		30);

	AddPart(
		std::make_unique<CharacterPart>(
			CharacterPartType::RightHand,
			Craft::PixelSprite(),
			Craft::Vector2(7, 8),
			3,
			true,
			false),
		30);
}

void Player::Move(float xDirection, float yDirection, float deltaTime)
{
	// 대각선 이동 보정
	if (xDirection != 0.0f && yDirection != 0.0f)
	{
		xDirection *= diagonalScale;
		yDirection *= diagonalScale;
	}

	float weaponMoveMultiplier = 1.0f;

	if (std::shared_ptr<WeaponBase> weapon = GetEquippedWeapon())
	{
		weaponMoveMultiplier = weapon->GetMoveSpeedMultiplier();
	}

	const float finalMoveSpeed = GetCharacterStats().moveSpeed * weaponMoveMultiplier;

	// x위치 업데이트
	// 이동 처리 -> 이동 방향과 빠르기를 적용해서 새로운 위치를 구하는 것
	// 이동 방향(direction) / 빠르기(moveSpeed) | 시간
	// 동속도 운동: 이동 거리 = 기존의 위치 + 이동 방향 x 빠르기 x 시간
	position.x += xDirection * finalMoveSpeed * deltaTime;
	position.y += yDirection * finalMoveSpeed * deltaTime;

	//* 화면 왼쪽 벗어나지 않도록 처리
	if (position.x < 0)
	{
		position.x = 0.0f;
	}

	// 화면 오른쪽 벗어나지 않도록 처리
	if (position.x + GetWidth() >= 1080.0f)
	{
		position.x = 1080.0f - GetWidth();
	}

	// 화면 위쪽 벗어나지 않도록 처리
	if (position.y < 0)
	{
		position.y = 0.0f;
	}

	// 화면 아래쪽 벗어나지 않도록 처리
	if (position.y + GetHeight() >= 960.0f)
	{
		position.y = 960.0f - GetHeight();
	}

	// 위치 업데이트
	SetPosition(position);
}

void Player::UpdateFacingDirection()
{
	// Player의 월드 중심 위치 계산
	const Craft::Vector2F playerPosition = GetPosition();

	const Craft::Vector2F playerCenterPosition(
		playerPosition.x + static_cast<float>(GetWidth()) * 0.5f,
		playerPosition.y + static_cast<float>(GetHeight()) * 0.5f);

	// 월드 좌표 → 화면 좌표 | Player 화면 좌표로 변환
	const Craft::Vector2 playerScreenPosition = Renderer::Get().GetCamera().WorldToScreen(playerCenterPosition);

	// 현재 마우스 화면 좌표
	const Craft::Vector2& mousePosition = Input::Get().GetMousePosition();

	// 변경 전 방향 저장
	const bool previousFacing = isFacingRight;

	// 좌 / 우 판정 [x가 정확히 같으면 기존 방향 유지]
	if (mousePosition.x > playerScreenPosition.x)
	{
		isFacingRight = true;
	}
	else if (mousePosition.x < playerScreenPosition.x)
	{
		isFacingRight = false;
	}

	// 방향 변화가 없다면 건너뛰기
	if (previousFacing == isFacingRight)
	{
		return;
	}

	if (isDiving)
	{
		// 다이브 중에는 머리만 방향 갱신
		visual.SetHeadFacingRight(isFacingRight);
	}
	else
	{
		// 실제 해당 부위 방향 갱신
		visual.SetFacingRight(isFacingRight);
	}

	// 현재 애니메이션 프레임을 원본부터 다시 적용한 뒤 새 방향으로 좌우 반전
	animator.RedrawCurrentFrame();
}

void Player::StartDive(float xDirection, float yDirection)
{
	// 예외 처리
	if (isDiving)
	{
		return;
	}

	// 이동 입력이 있다면 해당 이동 방향으로 Dive
	diveDirection = Craft::Vector2F(xDirection, yDirection);

	// 이동 입력 없다면 바라보는 방향으로 다이브
	if (xDirection == 0.0f && yDirection == 0.0f)
	{
		diveDirection.x = isFacingRight ? 1.0f : -1.0f;
		diveDirection.y = 0.0f;
	}

	const float length = std::sqrt(diveDirection.x * diveDirection.x + diveDirection.y * diveDirection.y);

	if (length > 0.0f)
	{
		diveDirection.x /= length;
		diveDirection.y /= length;
	}

	visual.SetDiveDirection(diveDirection);

	isDiving = true;

	diveTimer = 0.0f;

	// 다이브 시작과 동시에 무적 활성화
	isInvincible = true;

	invincibleTimer = 0.0f;

	// 애니메이션 즉시 시작
	animator.SetAnimationState(PlayerAnimationState::Dive);
}

void Player::UpdateDive(float deltaTime)
{
	if (!isDiving)
	{
		return;
	}

	position.x += diveDirection.x * diveSpeed * deltaTime;
	position.y += diveDirection.y * diveSpeed * deltaTime;

	//* 화면 왼쪽 벗어나지 않도록 처리
	if (position.x < 0)
	{
		position.x = 0.0f;
	}

	// 화면 오른쪽 벗어나지 않도록 처리
	if (position.x + GetWidth() >= 1080.0f)
	{
		position.x = 1080.0f - GetWidth();
	}

	// 화면 위쪽 벗어나지 않도록 처리
	if (position.y < 0)
	{
		position.y = 0.0f;
	}

	// 화면 아래쪽 벗어나지 않도록 처리
	if (position.y + GetHeight() >= 960.0f)
	{
		position.y = 960.0f - GetHeight();
	}

	SetPosition(position);

	// 다이브 시간
	diveTimer += deltaTime;

	// 무적 시간
	if (isInvincible)
	{
		invincibleTimer += deltaTime;

		if (invincibleTimer >= diveInvincibleDuration)
		{
			isInvincible = false;
		}
	}

	// 다이브 종료
	if (diveTimer >= diveDuration)
	{
		EndDive();
	}
}

void Player::EndDive()
{
	isDiving = false;
	isInvincible = false;

	diveTimer = 0.0f;
	invincibleTimer = 0.0f;

	diveDirection = Craft::Vector2F::Zero;
}

void Player::TryPickupWeapon()
{
	// 플레이어를 소유하고 있는 Level
	std::shared_ptr<Craft::Level> level = GetOwner();

	// 예외 처리
	if (!level)
	{
		return;
	}

	// 레벨에서 파생클래스들 찾기
	const std::vector<std::shared_ptr<WeaponBase>> weapons = level->FindActors<WeaponBase>();

	// 가장 가까운 무기
	std::shared_ptr<WeaponBase> nearestWeapon = nullptr;

	float nearestDistanceSquared = weaponPickupRange * weaponPickupRange;

	// 플레이어 현재 위치
	const Craft::Vector2F playerPosition = GetPosition();

	// 가장 가까운 무기 찾기
	for (const std::shared_ptr<WeaponBase>& weapon : weapons)
	{
		if (!weapon)
		{
			continue;
		}

		// 이미 장착된 무기 제외
		if (!weapon->IsDropped())
		{
			continue;
		}

		const Craft::Vector2F weaponPosition = weapon->GetPosition();

		// 플레이어와의 거리
		const float deltaX = weaponPosition.x - playerPosition.x;
		const float deltaY = weaponPosition.y - playerPosition.y;

		const float distanceSquared = deltaX * deltaX + deltaY * deltaY;

		// Pickup 범위 밖이면 제외
		if (distanceSquared > nearestDistanceSquared)
		{
			continue;
		}

		// 현재까지 가장 가까운 무기로 갱신
		nearestDistanceSquared = distanceSquared;
		nearestWeapon = weapon;
	}

	// 무기 장착
	if (!nearestWeapon)
	{
		return;
	}

	EquipWeapon(nearestWeapon);
}

Craft::Vector2F Player::GetAimDirection() const
{
	// 현재 마우스는 화면 기준 좌표
	const Craft::Vector2 mouseScreen = Input::Get().GetMousePosition();

	// Camera Position = 화면 좌상단의 월드 좌표
	const Craft::Vector2F cameraPosition = Renderer::Get().GetCamera().GetPosition();

	// Screen -> World
	const Craft::Vector2F mouseWorld(
		cameraPosition.x + static_cast<float>(mouseScreen.x),
		cameraPosition.y + static_cast<float>(mouseScreen.y)
	);

	// Player 중심
	const Craft::Vector2F playerCenter(
		position.x + static_cast<float>(GetWidth()) * 0.5f,
		position.y + static_cast<float>(GetHeight()) * 0.5f
	);

	Craft::Vector2F direction = mouseWorld - playerCenter;

	// 정규화
	const float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

	if (length <= 0.000001f)
	{
		return Craft::Vector2F::Zero;
	}

	direction.x /= length;
	direction.y /= length;

	return direction;
}
