#include "TestActor.h"
#include <Input/Input.h>

namespace Craft
{
	TestActor::TestActor()
		: Actor(Craft::Vector2F(120.0f, 50.0f))
	{
		sortingOrder = 10;

		SpriteTest();
	}

	void TestActor::Tick(float deltaTime)
	{
		super::Tick(deltaTime);

		// ----------------------------------------------------
		// 방향키 이동 테스트
		//
		// GetKey()를 사용하므로
		// 키를 누르고 있는 동안 계속 이동한다.
		// ----------------------------------------------------
		isMoving = false;

		if (Craft::Input::Get().GetKey(VK_LEFT))
		{
			position.x -= 1.0f;
			isMoving = true;
		}

		if (Craft::Input::Get().GetKey(VK_RIGHT))
		{
			position.x += 1.0f;
			isMoving = true;
		}

		if (Craft::Input::Get().GetKey(VK_UP))
		{
			position.y -= 1.0f;
			isMoving = true;
		}

		if (Craft::Input::Get().GetKey(VK_DOWN))
		{
			position.y += 1.0f;
			isMoving = true;
		}

		// ESC로 프로그램 종료 테스트
		if (Craft::Input::Get().GetKeyDown(VK_ESCAPE))
		{
			QuitGame();
		}
	}

	void TestActor::SpriteTest()
	{
		Craft::PixelSprite playerSprite(9, 15);

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


		auto MakePixel = [](const Craft::ColorRGB& color)
			{
				Craft::Cell cell;

				cell.character = ' ';
				cell.foreground = color;
				cell.background = color;

				return cell;
			};


		const Craft::Cell outlinePixel = MakePixel(outlineColor);

		const Craft::Cell helmetPixel = MakePixel(helmetColor);

		const Craft::Cell helmetLightPixel = MakePixel(helmetLightColor);

		const Craft::Cell visorPixel = MakePixel(visorColor);

		const Craft::Cell armorPixel = MakePixel(armorColor);

		const Craft::Cell armorLightPixel = MakePixel(armorLightColor);

		const Craft::Cell yellowPixel = MakePixel(yellowColor);

		const Craft::Cell legPixel = MakePixel(legColor);

		const Craft::Cell bootPixel = MakePixel(bootColor);

		const char* playerPixels[15] =
		{
			"..OOOOOO.", // 0  머리
			".OHHYHHYO", // 1
			"OHHhhhhhO", // 2
			"OHHVVVVVO", // 3
			"OHHHVVVVO", // 4
			"OHHHHYYHO", // 5
			"OHHHHHYHO", // 6

			".OYYYYAO.", // 7  목 / 상체 시작
			".OAAAAAO.", // 8
			".OAaaaYO.", // 9
			".OAAAAAO.", // 10
			".OAAAAAO.", // 11
			".OYYYYYO.", // 12

			".OL..OL..", // 13 다리
			".BBB.BBB."  // 14 부츠
		};

		for (int y = 0; y < 15; ++y)
		{
			for (int x = 0; x < 9; ++x)
			{
				switch (playerPixels[y][x])
				{
				case 'O':
					playerSprite.SetCell(x, y, outlinePixel);
					break;

				case 'H':
					playerSprite.SetCell(x, y, helmetPixel);
					break;

				case 'h':
					playerSprite.SetCell(x, y, helmetLightPixel);
					break;

				case 'V':
					playerSprite.SetCell(x, y, visorPixel);
					break;

				case 'A':
					playerSprite.SetCell(x, y, armorPixel);
					break;

				case 'a':
					playerSprite.SetCell(x, y, armorLightPixel);
					break;

				case 'Y':
					playerSprite.SetCell(x, y, yellowPixel);
					break;
				
				case 'L':
					playerSprite.SetCell(x, y, legPixel);
					break;
				
				case 'B':
					playerSprite.SetCell(x, y, bootPixel);
					break;

				case '.':
				default:
					// 투명 처리
					break;
				}
			}
		}
		ChangePixelSprite(playerSprite);
	}
}