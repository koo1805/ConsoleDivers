#include "GameLevel.h"

#include <Actor/Actor.h>
#include <Input/Input.h>
#include <Render/Cell.h>
#include <Render/Sprite/PixelSprite.h>
#include <Math/ColorRGB.h>
#include <Math/Vector2.h>

#include <Windows.h>

void GameLevel::OnInitialized()
{
	// 부모 Level 초기화
	Level::OnInitialized();

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


	// ----------------------------------------------------
	// PixelSprite Actor 생성
	// ----------------------------------------------------

	testActor = SpawnActor<Craft::Actor>(
		sprite,
		Craft::Vector2(10, 10)
	);
}


void GameLevel::Tick(float deltaTime)
{
	// 부모 Level의 Tick도 호출
	// Level이 가지고 있는 Actor들의 Tick 실행
	Level::Tick(deltaTime);

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


	// ESC로 프로그램 종료 테스트
	if (Craft::Input::Get().GetKeyDown(VK_ESCAPE))
	{
		testActor->QuitGame();
	}
}