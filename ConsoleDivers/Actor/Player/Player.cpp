#include "Player.h"
#include <Render/Renderer.h>
#include <Camera/Camera.h>
#include <Input/Input.h>

#include <memory>
#include <cmath>

using namespace Craft;
Player::Player()
	: Character(Craft::Vector2F(120.0f, 50.0f), CharacterStats{100, 80.0f})
{
	sortingOrder = 10;

	SetCharacterBounds(PlayerWidth, PlayerHeight);

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

	// ================================== 테스트 ===================================
	// 1 : 머리 파괴
	if (Input::Get().GetKeyDown('1'))
	{
		ApplyPartDamage(
			CharacterPartType::Head,
			9999);
	}
	// 2 : 머리 복구
	if (Input::Get().GetKeyDown('2'))
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

	// x위치 업데이트
	// 이동 처리 -> 이동 방향과 빠르기를 적용해서 새로운 위치를 구하는 것
	// 이동 방향(direction) / 빠르기(moveSpeed) | 시간
	// 동속도 운동: 이동 거리 = 기존의 위치 + 이동 방향 x 빠르기 x 시간
	position.x += xDirection * GetCharacterStats().moveSpeed * deltaTime;
	position.y += yDirection * GetCharacterStats().moveSpeed * deltaTime;

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
