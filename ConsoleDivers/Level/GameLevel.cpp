#include "GameLevel.h"

#include <Render/Renderer.h>
#include <Camera/Camera.h>
#include <Camera/Controller/CameraController.h>
#include <Debugging/DebugManager.h>
#include <Actor/Player/Player.h>

#include <Actor/Weapon/Shotgun/Shotgun.h>
#include <Actor/Enemy/NormalEnemy/NormalEnemy.h>

#include <Test/TestBG.h>

#include <Windows.h>
#include <cstdio>

using namespace Craft;
void GameLevel::OnInitialized()
{
	// 부모 Level 초기화
	Level::OnInitialized();

	testBGActor = SpawnActor<Craft::TestBG>();
	Craft::Renderer::Get().GetCamera().SetCameraClampSize(Craft::TestBG::GetWorldSize());

	//---------------------------------------------------------
	const Craft::Vector2 worldSize = Craft::TestBG::GetWorldSize();

	quadTree = std::make_unique<Craft::QuadTree>(
		Craft::QuadTreeBounds(
			0.0f,
			0.0f,
			static_cast<float>(worldSize.x),
			static_cast<float>(worldSize.y)),
		4);

	// QuadTree Debug 연결
	DebugManager::Get().SetQuadTreeDebugData(quadTree.get());

	constexpr int navigationCellSize = 10;

	const int gridWidth = worldSize.x / navigationCellSize;

	const int gridHeight =
		worldSize.y / navigationCellSize;

	navigationGrid.Initialize(
		gridWidth,
		gridHeight,
		navigationCellSize);

	// 테스트용 벽
	for (int y = 20; y < 50; ++y)
	{
		navigationGrid.SetWalkable(
			40,
			y,
			false);
	}

	// -----------------------------------------------------------

	player = SpawnActor<Player>();

	// NormalEnemy 생성
	normalEnemy = SpawnActor<NormalEnemy>(navigationGrid.GridToWorld(Craft::Vector2(35, 30)));

	// EnemyBase에 구현된 A* 기능이 사용할 NavigationGrid 연결
	if (normalEnemy)
	{
		normalEnemy->SetNavigationGrid(&navigationGrid);
	}

	SpawnActor<Shotgun>(Craft::Vector2F(player->GetPosition().x + 12.0f, player->GetPosition().y));

	cameraController = std::make_shared<Craft::CameraController>(Craft::Renderer::Get().GetCamera());
	cameraController->SetTargetPosition(player->GetPosition());

	cameraController->SnapToTarget();
	// 디버그 매니저에 카메라 정보를 연결
	DebugManager::Get().SetCameraController(cameraController.get());

	// A* 디버그 시각화에 사용할 데이터 연결
	if (normalEnemy)
	{
		DebugManager::Get().SetAStarDebugData(&navigationGrid, &normalEnemy->GetPathFinder());
	}
}


void GameLevel::Tick(float deltaTime)
{
	// 부모 Level의 Tick도 호출
	// Level이 가지고 있는 Actor들의 Tick 실행
	Level::Tick(deltaTime);

	if (quadTree)
	{
		// 이전 프레임의 공간 데이터 제거
		quadTree->Clear();

		// Player 삽입
		if (player && player->IsActive())
		{
			quadTree->Insert(player);
		}

		// Enemy 삽입
		if (normalEnemy && normalEnemy->IsActive())
		{
			quadTree->Insert(normalEnemy);
		}
	}

	if (quadTree && player)
	{
		const Craft::Vector2F playerPosition = player->GetPosition();

		// Player 기준 100 x 100 Query 영역
		const Craft::QuadTreeBounds queryBounds(playerPosition.x - 50.0f, playerPosition.y - 50.0f, 100.0f, 100.0f);

		// 현재 Query 영역에 포함되는 Actor 검색
		const std::vector<std::shared_ptr<Craft::Actor>> nearbyActors = quadTree->Query(queryBounds);

		// Query 디버그 시각화에 사용할 데이터 전달
		DebugManager::Get().SetQuadTreeQueryDebugData(queryBounds.x, queryBounds.y, queryBounds.width, queryBounds.height, nearbyActors);

		// 출력창 확인은 너무 자주 하지 않도록 0.5초에 한 번만 수행
		static float quadTreeDebugTimer = 0.0f;

		quadTreeDebugTimer += deltaTime;

		if (quadTreeDebugTimer >= 0.5f)
		{
			quadTreeDebugTimer = 0.0f;

			char buffer[128] = {};

			sprintf_s(
				buffer,
				sizeof(buffer),
				"[QuadTree] Nearby Actors: %zu\n",
				nearbyActors.size());

			OutputDebugStringA(buffer);
		}
	}

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

	// 변경된 위치 적용	
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
