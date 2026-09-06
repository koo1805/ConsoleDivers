#include "StratagemBeacon.h"
#include <Level/Level.h>
#include <Render/Cell.h>
#include <Math/ColorRGB.h>
#include <Collision/ConsoleDiversCollisionLayer.h>
#include <Stratagem/Hellpod/Hellpod.h>

#include <cmath>

using namespace Craft;

namespace
{
	// PixelSprite에 사용할 Cell 생성 보조 함수
	// ========================================================
	Craft::Cell MakeBeaconPixel(const Craft::ColorRGB& color)
	{
		Craft::Cell cell;

		// ConsoleDivers의 PixelSprite 방식: 문자 자체보다 배경색으로 픽셀을 표현
		cell.character = ' ';
		cell.foreground = color;
		cell.background = color;

		return cell;
	}
}

StratagemBeacon::StratagemBeacon(const Craft::Vector2F& position, const Craft::Vector2F& direction, const StratagemData& stratagemData)
	: Actor(position), stratagemData(stratagemData), startPosition(position), landingPosition(position)
{
	// 투척 방향 정규화
	SetDirection(direction);

	// 비콘 Sprite 설정
	ChangePixelSprite(CreateBeaconSprite());

	// Player / Weapon보다 위쪽에서 보이도록 설정
	sortingOrder = 25;

	// 비콘은 별도 Layer 사용
	// --------------------------------------------------------
	SetCollisionLayer(GameCollision::StratagemBeacon);

	// 현재는 World Actor와만 충돌하도록 준비
	SetCollisionMask(GameCollision::Mask(GameCollision::World));
}

void StratagemBeacon::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	switch (state)
	{
	case StratagemBeaconState::Flying:
		UpdateFlying(deltaTime);
		break;

	case StratagemBeaconState::Landed:
		UpdateLanded(deltaTime);
		break;

	case StratagemBeaconState::Called:
		UpdateCalled(deltaTime);
		break;

	default:
		break;
	}
}

void StratagemBeacon::UpdateFlying(float deltaTime)
{
	// 방향이 없다면 이동시킬 수 없음
	if (direction == Craft::Vector2F::Zero)
	{
		Land();
		return;
	}

	// 이번 프레임 이동 거리
	float moveDistance = throwSpeed * deltaTime;

	// maxThrowDistance를 넘어가는 마지막 프레임 보정
	// 남은 거리 = 2 | 이번 프레임 이동량 = 4
	// 그대로 이동하면 2만큼 초과하므로 남은 거리까지만 이동
	const float remainingDistance = maxThrowDistance - traveledDistance;

	if (moveDistance >= remainingDistance)
	{
		moveDistance = remainingDistance;
	}

	const Craft::Vector2F movement = direction * moveDistance;

	SetPosition(GetPosition() + movement);

	traveledDistance += moveDistance;

	// 최대 투척 거리 도달
	if (traveledDistance >= maxThrowDistance)
	{
		Land();
	}
}

void StratagemBeacon::UpdateLanded(float deltaTime)
{
	// 착지 후 경과시간
	callTimer += deltaTime;
	beaconTimer += deltaTime;

	// 신호 깜빡임 처리
	signalBlinkTimer += deltaTime;

	if (signalBlinkTimer >= signalBlinkInterval)
	{
		// interval 이상 누적됐으므로 초과분은 버리지 않고 빼주는 방식 사용
		signalBlinkTimer -= signalBlinkInterval;

		isSignalBright = !isSignalBright;

		ChangePixelSprite(CreateSignalSprite(isSignalBright));
	}

	// 아직 호출 대기시간이 남아있음
	if (callTimer < stratagemData.callDelay)
	{
		return;
	}

	// callDelay 도달
	// 여기서 실제 스트라타젬 호출 이벤트 발생
	CallStratagem();
}

void StratagemBeacon::UpdateCalled(float deltaTime)
{
	beaconTimer += deltaTime;

	// Called 상태에서도 빛기둥 점멸 유지
	signalBlinkTimer += deltaTime;

	if (signalBlinkTimer >= signalBlinkInterval)
	{
		signalBlinkTimer -= signalBlinkInterval;

		isSignalBright = !isSignalBright;

		ChangePixelSprite(CreateSignalSprite(isSignalBright));
	}

	// beaconDuration 동안 신호 유지
	if (beaconTimer < stratagemData.beaconDuration)
	{
		return;
	}

	// 신호 유지시간 종료
	Destroy();
}

void StratagemBeacon::Land()
{
	// 이미 착지한 경우 중복 처리 방지
	if (state == StratagemBeaconState::Landed)
	{
		return;
	}

	// 1. 착지 상태 전환
	// ========================================================
	state = StratagemBeaconState::Landed;

	// 현재 위치가 최종 호출 위치
	landingPosition = GetPosition();

	// 착지 이후에는 더 이상 World 충돌을 검사할 필요가 없음
	// 다음 단계에서 필요하다면 Landed 전용 Mask를 다시 설정할 수 있음
	SetCollisionMask(Craft::CollisionMaskNone);

	// 2. 호출 관련 Timer 초기화
	// ========================================================
	callTimer = 0.0f;
	beaconTimer = 0.0f;
	signalBlinkTimer = 0.0f;
	isSignalBright = true;

	// 3. 착지 즉시 신호 빛기둥 활성화
	// ========================================================
	ChangePixelSprite(CreateSignalSprite(true));

	// Signal의 마지막 행이 실제 착지 위치와 맞도록 Actor 표시 위치를 위쪽으로 이동
	ApplySignalPosition();
}

void StratagemBeacon::CallStratagem()
{
	// 중복 호출 방지
	if (state != StratagemBeaconState::Landed)
	{
		return;
	}

	// 실제 종류별 스트라타젬 실행
	ExecuteStratagem();

	// 호출 완료
	state = StratagemBeaconState::Called;
}

void StratagemBeacon::ExecuteStratagem()
{
	// ========================================================
	// 이 함수가 모든 스트라타젬 효과의 공통 진입점
	//
	// 비콘은:
	// "언제 호출할지"
	// "어디에 호출할지"
	//
	// 까지만 담당하고,
	//
	// 실제 종류별 효과는 type으로 분기한다.
	// ========================================================

	switch (stratagemData.type)
	{
	case StratagemType::SupportWeapon:
	{
		std::shared_ptr<Craft::Level> level = GetOwner();

		if (!level)
		{
			return;
		}

		level->SpawnActor<Hellpod>(landingPosition, stratagemData.id);

		break;
	}

	case StratagemType::Sentry:
	{
		// 추후: Sentry Hellpod 호출
		break;
	}

	case StratagemType::Offensive:
	{
		// 추후: 폭격 / 공격형 스트라타젬 호출
		break;
	}

	default:
		break;
	}
}

void StratagemBeacon::SetDirection(const Craft::Vector2F& newDirection)
{
	const float lengthSquared = newDirection.x * newDirection.x + newDirection.y * newDirection.y;

	if (lengthSquared <= 0.000001f)
	{
		direction = Craft::Vector2F::Zero;
		return;
	}

	const float length = std::sqrt(lengthSquared);

	direction = Craft::Vector2F(newDirection.x / length, newDirection.y / length);
}

void StratagemBeacon::ApplySignalPosition()
{
	const float signalOffsetY = static_cast<float>(SignalHeight - 1);

	SetPosition(Craft::Vector2F(landingPosition.x, landingPosition.y - signalOffsetY));
}

void StratagemBeacon::OnCollision(const std::shared_ptr<Craft::Actor>& other)
{
	if (!other)
	{
		return;
	}

	// 이미 착지했다면 처리할 필요 없음
	if (state != StratagemBeaconState::Flying)
	{
		return;
	}

	const Craft::CollisionMask otherLayerMask = Craft::ToCollisionMask(other->GetCollisionLayer());

	// World와 충돌하면 그 자리에서 착지
	if (Craft::HasCollisionLayer(otherLayerMask, GameCollision::World))
	{
		Land();
	}
}

Craft::PixelSprite StratagemBeacon::CreateBeaconSprite() const
{
	constexpr int width = 3;
	constexpr int height = 3;

	Craft::PixelSprite sprite(width, height);

	// 테스트용 비콘 색상
	// ========================================================
	const Craft::ColorRGB darkColor(45, 48, 45);

	const Craft::ColorRGB bodyColor(80, 85, 78);

	const Craft::ColorRGB signalColor(220, 60, 40);

	const Craft::Cell darkPixel = MakeBeaconPixel(darkColor);

	const Craft::Cell bodyPixel = MakeBeaconPixel(bodyColor);

	const Craft::Cell signalPixel = MakeBeaconPixel(signalColor);

	// ========================================================
	// 3 x 3 간단한 테스트 비콘
	//
	// . = 투명
	// R = 신호등
	// B = 몸체
	// D = 어두운 외곽
	//
	//   R
	//  DBD
	//   D
	// ========================================================
	const char* pixels[height] =
	{
		".R.",
		"DBD",
		".D."
	};

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			switch (pixels[y][x])
			{
			case 'R':
				sprite.SetCell(x, y, signalPixel);
				break;

			case 'B':
				sprite.SetCell(x, y, bodyPixel);
				break;

			case 'D':
				sprite.SetCell(x, y, darkPixel);
				break;

			case '.':
			default:
				break;
			}
		}
	}

	return sprite;
}

Craft::PixelSprite StratagemBeacon::CreateSignalSprite(bool bright) const
{
	constexpr int width = SignalWidth;
	constexpr int height = SignalHeight;

	Craft::PixelSprite sprite(width, height);

	const Craft::ColorRGB signalColor = bright ? Craft::ColorRGB(255, 75, 45) : Craft::ColorRGB(135, 35, 25);

	const Craft::ColorRGB coreColor = bright ? Craft::ColorRGB(255, 205, 90) : Craft::ColorRGB(165, 85, 35);

	const Craft::Cell signalPixel = MakeBeaconPixel(signalColor);

	const Craft::Cell corePixel = MakeBeaconPixel(coreColor);

	// ========================================================
	// 3 x 20 신호 빛기둥
	//
	// C : 중심 광선
	// S : 외곽 신호
	// . : 투명
	//
	// 위쪽은 얇은 직선 형태로 길게 유지하고,
	// 아래쪽만 조금 퍼지도록 구성.
	// ========================================================
	const char* pixels[height] =
	{
		".C.",
		".C.",
		".C.",
		".C.",
		".C.",
		".C.",
		".C.",
		".C.",
		".C.",
		".C.",
		".C.",
		".C.",
		".C.",
		".C.",
		".C.",
		".C.",
		"SCS",
		".C.",
		"SCS",
		"SCS"
	};

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			switch (pixels[y][x])
			{
			case 'C':
				sprite.SetCell(x, y, corePixel);
				break;

			case 'S':
				sprite.SetCell(x, y, signalPixel);
				break;

			default:
				break;
			}
		}
	}

	return sprite;
}