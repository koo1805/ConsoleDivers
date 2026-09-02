#include "Player.h"
#include <Engine/Engine.h>
#include <Input/Input.h>

#include <memory>

using namespace Craft;

namespace
{
	Craft::Cell MakePixel(const Craft::ColorRGB& color)
	{
		Craft::Cell cell;

		cell.character = ' ';
		cell.foreground = color;
		cell.background = color;

		return cell;
	};

	Craft::PixelSprite MakePartSprite(
		int width,
		int height,
		const char* const* pixels,
		const Craft::Cell& outlinePixel,
		const Craft::Cell& helmetPixel,
		const Craft::Cell& helmetLightPixel,
		const Craft::Cell& visorPixel,
		const Craft::Cell& armorPixel,
		const Craft::Cell& armorLightPixel,
		const Craft::Cell& yellowPixel,
		const Craft::Cell& legPixel,
		const Craft::Cell& bootPixel)
	{
		Craft::PixelSprite sprite(width, height);

		for (int y = 0; y < 15; ++y)
		{
			for (int x = 0; x < 9; ++x)
			{
				switch (pixels[y][x])
				{
				case 'O':
					sprite.SetCell(x, y, outlinePixel);
					break;

				case 'H':
					sprite.SetCell(x, y, helmetPixel);
					break;

				case 'h':
					sprite.SetCell(x, y, helmetLightPixel);
					break;

				case 'V':
					sprite.SetCell(x, y, visorPixel);
					break;

				case 'A':
					sprite.SetCell(x, y, armorPixel);
					break;

				case 'a':
					sprite.SetCell(x, y, armorLightPixel);
					break;

				case 'Y':
					sprite.SetCell(x, y, yellowPixel);
					break;

				case 'L':
					sprite.SetCell(x, y, legPixel);
					break;

				case 'B':
					sprite.SetCell(x, y, bootPixel);
					break;

				case '.':
				default:
					// 투명 처리
					break;
				}
			}
		}

		return sprite;
	}


}

Player::Player()
	: Character(Craft::Vector2F(120.0f, 50.0f), CharacterStats{100, 80.0f})
{
	sortingOrder = 10;

	SetCharacterBounds(PlayerWidth, PlayerHeight);

	// 부위 Sprite 생성 및 Character에 등록
	PlayerPartsGenerate();
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
	if (Craft::Input::Get().GetKey('A'))
	{
		xDirection = -1.0f;
		isMoving = true;
	}

	// Right
	if (Craft::Input::Get().GetKey('D'))
	{
		xDirection = 1.0f;
		isMoving = true;
	}

	// Up
	if (Craft::Input::Get().GetKey('W'))
	{
		yDirection = -1.0f;
		isMoving = true;
	}

	// Down
	if (Craft::Input::Get().GetKey('S'))
	{
		yDirection = 1.0f;
		isMoving = true;
	}

	// 이동 함수 호출
	Move(xDirection, yDirection, deltaTime);

	// ESC로 프로그램 종료 테스트
	if (Craft::Input::Get().GetKeyDown(VK_ESCAPE))
	{
		QuitGame();
	}
}

void Player::PlayerPartsGenerate()
{
	// ----------------------------------------------------
	// 색상
	// ----------------------------------------------------

	const Craft::ColorRGB outlineColor(20, 22, 20);     // 외곽선
	const Craft::ColorRGB helmetColor(45, 49, 45);      // 헬멧
	const Craft::ColorRGB helmetLightColor(68, 72, 65); // 헬멧 밝은 면
	const Craft::ColorRGB visorColor(40, 85, 95);       // 어두운 바이저

	const Craft::ColorRGB armorColor(58, 62, 57);       // 방탄복
	const Craft::ColorRGB armorLightColor(82, 86, 78);  // 방탄복 밝은 면

	const Craft::ColorRGB yellowColor(220, 185, 45);    // 노란 포인트

	const Craft::ColorRGB legColor(45, 48, 44);         // 다리
	const Craft::ColorRGB bootColor(26, 28, 26);        // 부츠

	// Cell 변환
	const Craft::Cell outlinePixel = MakePixel(outlineColor);

	const Craft::Cell helmetPixel = MakePixel(helmetColor);

	const Craft::Cell helmetLightPixel = MakePixel(helmetLightColor);

	const Craft::Cell visorPixel = MakePixel(visorColor);

	const Craft::Cell armorPixel = MakePixel(armorColor);

	const Craft::Cell armorLightPixel = MakePixel(armorLightColor);

	const Craft::Cell yellowPixel = MakePixel(yellowColor);

	const Craft::Cell legPixel = MakePixel(legColor);

	const Craft::Cell bootPixel = MakePixel(bootColor);

	// ========================================================
	// Head
	//
	// 크기: 9 x 7
	// Character 기준 위치: (0, 0)
	// ========================================================
	const char* headPixels[7] =
	{
		"..OOOOOO.",
		".OHHYHHYO",
		"OHHhhhhhO",
		"OHHVVVVVO",
		"OHHHVVVVO",
		"OHHHHYYHO",
		"OHHHHHYHO"
	};

	// ========================================================
	// Body
	//
	// 기존 Player Sprite의 7~12행.
	//
	// 양쪽 외곽선은 LeftHand / RightHand 쪽으로 분리했다.
	// ========================================================
	const char* bodyPixels[6] =
	{
		".OYYYYAO.",
		".OAAAAAO.",
		".OAaaaYO.",
		".OAAAAAO.",
		".OAAAAAO.",
		".OYYYYYO."
	};


	// ========================================================
	// Legs
	//
	// 기존 Player Sprite의 13~14행
	// ========================================================
	const char* legsPixels[2] =
	{
		".OL..OL..",
		".BBB.BBB."
	};

	// Head
	const Craft::PixelSprite headSprite =
		MakePartSprite(
			9,
			7,
			headPixels,
			outlinePixel,
			helmetPixel,
			helmetLightPixel,
			visorPixel,
			armorPixel,
			armorLightPixel,
			yellowPixel,
			legPixel,
			bootPixel);


	// Body
	const Craft::PixelSprite bodySprite =
		MakePartSprite(
			9,
			6,
			bodyPixels,
			outlinePixel,
			helmetPixel,
			helmetLightPixel,
			visorPixel,
			armorPixel,
			armorLightPixel,
			yellowPixel,
			legPixel,
			bootPixel);

	// Legs
	const Craft::PixelSprite legsSprite =
		MakePartSprite(
			9,
			2,
			legsPixels,
			outlinePixel,
			helmetPixel,
			helmetLightPixel,
			visorPixel,
			armorPixel,
			armorLightPixel,
			yellowPixel,
			legPixel,
			bootPixel);

	AddPart(
		std::make_unique<CharacterPart>(
			CharacterPartType::Legs,
			legsSprite,
			Craft::Vector2(0, 13),
			0,
			true,
			false),
		40);

	AddPart(
		std::make_unique<CharacterPart>(
			CharacterPartType::Body,
			bodySprite,
			Craft::Vector2(0, 7),
			1,
			false,
			true),
		100);

	AddPart(
		std::make_unique<CharacterPart>(
			CharacterPartType::Head,
			headSprite,
			Craft::Vector2(0, 0),
			3,
			true,
			false),
		50);
}

void Player::Move(float xDirection, float yDirection, float deltaTime)
{
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
