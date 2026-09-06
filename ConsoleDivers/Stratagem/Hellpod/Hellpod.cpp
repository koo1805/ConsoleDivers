#include "Hellpod.h"

#include <Level/Level.h>

#include <Actor/Weapon/ArcThrower/ArcThrower.h>

#include <Level/GameLevel.h>

#include <Math/ColorRGB.h>
#include <Render/Cell.h>

using namespace Craft;

namespace
{
	Craft::Cell MakeHellpodPixel(const Craft::ColorRGB& color)
	{
		Craft::Cell cell;

		cell.character = ' ';
		cell.foreground = color;
		cell.background = color;

		return cell;
	}
}

Hellpod::Hellpod(const Craft::Vector2F& targetPosition, StratagemId stratagemId)
	:Actor(Craft::Vector2F(targetPosition.x, targetPosition.y - spawnHeight)),
	targetPosition(targetPosition),
	stratagemId(stratagemId)
{
	// Hellpod Sprite
	ChangePixelSprite(CreateHellpodSprite());

	// 다른 Weapon / Beacon보다 위에 보이도록
	sortingOrder = 30;

	// 현재는 Hellpod 자체 충돌을 사용하지 않음
	SetCollisionMask(Craft::CollisionMaskNone);
}

void Hellpod::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	switch (state)
	{
	case HellpodState::Falling:
		UpdateFalling(deltaTime);
		break;

	case HellpodState::Landed:
		// 현재 단계에서는 착지 즉시 Payload를 Spawn하고
		// Hellpod을 제거하므로 별도 Update 불필요
		break;

	default:
		break;
	}
}

void Hellpod::UpdateFalling(float deltaTime)
{
	Craft::Vector2F currentPosition = GetPosition();

	// 이번 프레임 낙하 거리
	const float fallDistance = fallSpeed * deltaTime;

	currentPosition.y += fallDistance;

	// 목표 Y를 지나쳤다면 정확히 목표 위치에 고정
	if (currentPosition.y >= targetPosition.y)
	{
		SetPosition(targetPosition);

		Land();

		return;
	}

	SetPosition(currentPosition);
}

void Hellpod::Land()
{
	if (state != HellpodState::Falling)
	{
		return;
	}

	state = HellpodState::Landed;

	// 착지 위치를 정확히 보정
	SetPosition(targetPosition);

	// Hellpod 내용물 생성
	SpawnPayload();

	// ========================================================
	// 현재 단계에서는 Hellpod이 열리는 애니메이션이 없으므로
	// 내용물을 Spawn한 뒤 제거.
	//
	// 추후:
	// Landed → Opening → Opened
	// 구조로 바꾸면 여기서 바로 Destroy하지 않으면 된다.
	// ========================================================
	Destroy();
}

void Hellpod::SpawnPayload()
{
	// 중복 Spawn 방지
	if (hasSpawnedPayload)
	{
		return;
	}

	std::shared_ptr<Craft::Level> level = GetOwner();

	if (!level)
	{
		return;
	}

	switch (stratagemId)
	{
	case StratagemId::ArcThrower:
	{
		// ====================================================
		// ArcThrower를 Dropped 상태로 Spawn
		//
		// WeaponBase 생성자는 기본적으로 Dropped 상태이며
		// Player CollisionMask를 가지므로
		// 기존 E 줍기 시스템을 그대로 사용할 수 있다.
		// ====================================================
		std::shared_ptr<ArcThrower> arcThrower = level->SpawnActor<ArcThrower>(targetPosition);

		if (!arcThrower)
		{
			return;
		}

		// ====================================================
		// ArcThrower는 QuadTree와 NavigationGrid가 필요.
		//
		// GameLevel에 연결 책임을 위임한다.
		// ====================================================
		std::shared_ptr<GameLevel> gameLevel = std::dynamic_pointer_cast<GameLevel>(level);

		if (gameLevel)
		{
			gameLevel->InitializeArcThrower(arcThrower);
		}

		break;
	}

	case StratagemId::None:
	default:
		return;
	}

	// 실제 Spawn 성공
	hasSpawnedPayload = true;
}

Craft::PixelSprite Hellpod::CreateHellpodSprite() const
{
	constexpr int width = 5;
	constexpr int height = 7;

	Craft::PixelSprite sprite(width, height);

	// ========================================================
	// Hellpod 색상
	// ========================================================
	const Craft::ColorRGB darkMetalColor(35, 38, 36);

	const Craft::ColorRGB metalColor(78, 82, 76);

	const Craft::ColorRGB bodyColor(50, 54, 51);

	const Craft::ColorRGB yellowColor(220, 185, 45);

	const Craft::Cell darkPixel =MakeHellpodPixel(darkMetalColor);

	const Craft::Cell metalPixel =MakeHellpodPixel(metalColor);

	const Craft::Cell bodyPixel =MakeHellpodPixel(bodyColor);

	const Craft::Cell yellowPixel =MakeHellpodPixel(yellowColor);

	// ========================================================
	// Hellpod 간단한 형태
	//
	// . 투명
	// D Dark
	// M Metal
	// B Body
	// Y Yellow
	// ========================================================
	const char* pixels[height] =
	{
		"..M..",
		".MMM.",
		"DBBBD",
		"DBYBD",
		"DBBBD",
		".DDD.",
		"..D.."
	};

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			switch (pixels[y][x])
			{
			case 'D':
				sprite.SetCell(x, y, darkPixel);
				break;

			case 'M':
				sprite.SetCell(x, y, metalPixel);
				break;

			case 'B':
				sprite.SetCell(x, y, bodyPixel);
				break;

			case 'Y':
				sprite.SetCell(x, y, yellowPixel);
				break;

			default:
				break;
			}
		}
	}

	return sprite;
}