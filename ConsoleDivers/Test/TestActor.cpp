#include "TestActor.h"
#include <Input/Input.h>

namespace Craft
{
	TestActor::TestActor()
		: Actor(Craft::Vector2F(120.0f, 50.0f))
	{
		sortingOrder = 10;

		// ----------------------------------------------------
		// 테스트용 PixelSprite 생성
		//
		// 5 x 3 크기의 작은 캐릭터를 만든다.
		//
		//   █ █
		//   ███
		//   █ █
		//
		// 문자 자체는 공백(' ')을 사용하고
		// background RGB 색상을 이용해서 픽셀처럼 표현한다.
		// ----------------------------------------------------

		Craft::PixelSprite sprite(5, 3);

		// 빨간색
		const Craft::ColorRGB red(255, 60, 60);

		// 주황색
		const Craft::ColorRGB orange(255, 150, 40);

		// 검은색
		const Craft::ColorRGB black(0, 0, 0);


		// ----------------------------------------------------
		// Cell 생성
		// ----------------------------------------------------

		Craft::Cell redCell;
		redCell.character = ' ';
		redCell.foreground = red;
		redCell.background = red;

		Craft::Cell orangeCell;
		orangeCell.character = ' ';
		orangeCell.foreground = orange;
		orangeCell.background = orange;


		// ----------------------------------------------------
		// PixelSprite 모양 설정
		//
		//  좌표
		//
		//  0 1 2 3 4
		//
		//  . R . R .     y = 0
		//  . R O R .     y = 1
		//  . R . R .     y = 2
		//
		// '.'은 투명 Cell
		// ----------------------------------------------------

		// 머리 / 위쪽
		sprite.SetCell(1, 0, redCell);
		sprite.SetCell(3, 0, redCell);

		// 몸통
		sprite.SetCell(1, 1, redCell);
		sprite.SetCell(2, 1, orangeCell);
		sprite.SetCell(3, 1, redCell);

		// 다리
		sprite.SetCell(1, 2, redCell);
		sprite.SetCell(3, 2, redCell);

		ChangePixelSprite(sprite);
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
		Craft::PixelSprite playerSprite(9, 18);

		const Craft::ColorRGB outlineColor(56, 32, 84);   // 진한 보라 외곽
		const Craft::ColorRGB purpleColor(106, 74, 145);  // 기본 보라
		const Craft::ColorRGB goldColor(232, 214, 120);   // 금색 장식
		const Craft::ColorRGB eyeColor(55, 220, 255);     // 청록 눈
		const Craft::ColorRGB bodyColor(74, 54, 102);     // 몸통
		const Craft::ColorRGB legColor(66, 48, 90);       // 다리
		const Craft::ColorRGB bootColor(48, 40, 64);      // 부츠

		auto MakePixel = [](const Craft::ColorRGB& color)
			{
				Craft::Cell cell;
				cell.character = ' ';      // 중요: '\0' 이 아니어야 보임
				cell.foreground = color;
				cell.background = color;
				return cell;
			};

		const Craft::Cell outlinePixel = MakePixel(outlineColor);
		const Craft::Cell purplePixel = MakePixel(purpleColor);
		const Craft::Cell goldPixel = MakePixel(goldColor);
		const Craft::Cell eyePixel = MakePixel(eyeColor);
		const Craft::Cell bodyPixel = MakePixel(bodyColor);
		const Craft::Cell legPixel = MakePixel(legColor);
		const Craft::Cell bootPixel = MakePixel(bootColor);

		const char* playerPixels[18] =
		{
			"...DGD...",  // 0
			"..DDGDD..",  // 1
			".DDPGPDD.",  // 2
			".DPPGPPD.",  // 3
			"DDPCCCPDD",  // 4
			"DDCC.CCDD",  // 5
			"DPGPPPGPD",  // 6
			"DGGPPPGGD",  // 7
			".DPPPPPD.",  // 8

			".DBBBBBD.",  // 9
			".DBBBBBD.",  // 10
			"..BBBBB..",  // 11
			".BBGPGGB.",  // 12

			"..L...L..",  // 13
			"..L...L..",  // 14
			"..LG.GL..",  // 15
			".LL...LL.",  // 16
			".FF...FF."   // 17
		};


		for (int y = 0; y < 18; ++y)
		{
			for (int x = 0; x < 9; ++x)
			{
				switch (playerPixels[y][x])
				{
				case 'D':
					playerSprite.SetCell(x, y, outlinePixel);
					break;

				case 'P':
					playerSprite.SetCell(x, y, purplePixel);
					break;

				case 'G':
					playerSprite.SetCell(x, y, goldPixel);
					break;

				case 'C':
					playerSprite.SetCell(x, y, eyePixel);
					break;

				case 'B':
					playerSprite.SetCell(x, y, bodyPixel);
					break;

				case 'L':
					playerSprite.SetCell(x, y, legPixel);
					break;

				case 'F':
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