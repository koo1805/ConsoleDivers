#include "GameLevel.h"

#include <Actor/Actor.h>
#include <Input/Input.h>
#include <Render/Renderer.h>
#include <Render/Cell.h>
#include <Render/Sprite/PixelSprite.h>
#include <Camera/Camera.h>
#include <Math/ColorRGB.h>
#include <Math/Vector2.h>

#include <Windows.h>

void GameLevel::OnInitialized()
{
	// 부모 Level 초기화
	Level::OnInitialized();

	// ============================================================
	// 1. 월드 배경 생성
	// ============================================================

	// 화면은 120 x 40이지만
	// 실제 월드는 그보다 훨씬 크게 만든다.
	const int worldWidth = 1080;
	const int worldHeight = 960;

	Craft::PixelSprite background(worldWidth, worldHeight);


	// ------------------------------------------------------------
	// 배경 색상
	// ------------------------------------------------------------

	const Craft::ColorRGB darkGray(30, 30, 30);
	const Craft::ColorRGB gray(55, 55, 55);
	const Craft::ColorRGB blue(30, 80, 160);


	Craft::Cell floorCell;
	floorCell.character = ' ';
	floorCell.foreground = darkGray;
	floorCell.background = darkGray;


	Craft::Cell gridCell;
	gridCell.character = ' ';
	gridCell.foreground = gray;
	gridCell.background = gray;


	Craft::Cell markerCell;
	markerCell.character = ' ';
	markerCell.foreground = blue;
	markerCell.background = blue;


	// ============================================================
	// 2. 배경 채우기
	// ============================================================

	for (int y = 0; y < worldHeight; ++y)
	{
		for (int x = 0; x < worldWidth; ++x)
		{
			Craft::Cell cell = floorCell;


			// ----------------------------------------------------
			// 10칸마다 격자선
			// ----------------------------------------------------

			if (x % 10 == 0 || y % 5 == 0)
			{
				cell = gridCell;
			}


			// ----------------------------------------------------
			// 50칸마다 더 눈에 잘 띄는 기준선
			// ----------------------------------------------------

			if (x % 50 == 0 || y % 25 == 0)
			{
				cell = markerCell;
			}


			background.SetCell(x, y, cell);
		}
	}

	testBGActor = SpawnActor<Craft::Actor>(background, Craft::Vector2(0, 0));

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

	// ----------------------------------------------------
	// PixelSprite Actor 생성
	// ----------------------------------------------------

	testActor = SpawnActor<Craft::Actor>(
		sprite,
		Craft::Vector2(120, 50)
	);

	// 처음부터 플레이어를 화면 중앙으로 맞춰준다.
	Craft::Renderer::Get()
		.GetCamera()
		.CenterOn(testActor->GetPosition());
}


void GameLevel::Tick(float deltaTime)
{
	// 부모 Level의 Tick도 호출
	// Level이 가지고 있는 Actor들의 Tick 실행
	Level::Tick(deltaTime);

	// 프레임 관련 문자열.
	const int size = 256;
	char fpsString[size] = {};
	sprintf_s(
		fpsString,
		size,
		"dt: %f | fps: %.1f",
		deltaTime,
		(1.0f / deltaTime)
	);

	// 콘솔 창 이름에 값 설정.
	SetConsoleTitleA(fpsString);

	// Actor가 없으면 종료
	if (!testActor)
	{
		return;
	}


	// ----------------------------------------------------
	// 현재 위치
	// ----------------------------------------------------

	Craft::Vector2 position = testActor->GetPosition();


	// ----------------------------------------------------
	// 방향키 이동 테스트
	//
	// GetKey()를 사용하므로
	// 키를 누르고 있는 동안 계속 이동한다.
	// ----------------------------------------------------

	if (Craft::Input::Get().GetKey(VK_LEFT))
	{
		position.x -= 1;
	}

	if (Craft::Input::Get().GetKey(VK_RIGHT))
	{
		position.x += 1;
	}

	if (Craft::Input::Get().GetKey(VK_UP))
	{
		position.y -= 1;
	}

	if (Craft::Input::Get().GetKey(VK_DOWN))
	{
		position.y += 1;
	}


	// ----------------------------------------------------
	// 변경된 위치 적용
	// ----------------------------------------------------

	testActor->SetPosition(position);

	// 플레이어 위치를 화면 중앙으로 유지
	Craft::Renderer::Get().GetCamera().CenterOn(testActor->GetPosition());


	// ESC로 프로그램 종료 테스트
	if (Craft::Input::Get().GetKeyDown(VK_ESCAPE))
	{
		testActor->QuitGame();
	}
}