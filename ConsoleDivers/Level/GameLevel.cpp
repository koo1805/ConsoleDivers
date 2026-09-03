#include "GameLevel.h"

#include <Render/Renderer.h>
#include <Camera/Camera.h>
#include <Camera/Controller/CameraController.h>
#include <Debugging/DebugManager.h>
#include <Actor/Player/Player.h>

#include <Actor/Weapon/Shotgun/Shotgun.h>

#include <Test/TestBG.h>

#include <Windows.h>

using namespace Craft;
void GameLevel::OnInitialized()
{
	// 부모 Level 초기화
	Level::OnInitialized();

	testBGActor = SpawnActor<Craft::TestBG>();
	Craft::Renderer::Get().GetCamera().SetCameraClampSize(Craft::TestBG::GetWorldSize());

	player = SpawnActor<Player>();

	SpawnActor<Shotgun>(Craft::Vector2F(player->GetPosition().x + 12.0f, player->GetPosition().y));

	cameraController = std::make_shared<Craft::CameraController>(Craft::Renderer::Get().GetCamera());
	cameraController->SetTargetPosition(player->GetPosition());

	cameraController->SnapToTarget();
	// 디버그 매니저에 카메라 정보를 연결
	DebugManager::Get().SetCameraController(cameraController.get());
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
	if (!player || !cameraController)
	{
		return;
	}

	// ----------------------------------------------------
	// 변경된 위치 적용
	// ----------------------------------------------------
	
	cameraController->SetTargetPosition(player->GetPosition());
	cameraController->SetTargetMoving(player->IsMoving());
	cameraController->Tick(deltaTime);

	// 디버그 상태 변경시 업데이트
	DebugManager::Get().Tick(deltaTime);
}

void GameLevel::Draw()
{
	super::Draw();

	// 디버그 그리기 명령 생성
	DebugManager::Get().Draw();
}
