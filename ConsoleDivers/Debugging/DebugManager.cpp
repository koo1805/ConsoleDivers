#include "DebugManager.h"
#include <Camera/Camera.h>
#include <Camera/Controller/CameraController.h>
#include <Render/Renderer.h>
#include <Render/Debugging/DebugRenderer.h>
#include <Input/Input.h>
#include <Algorithm/AStar/AStarPathFinder.h>
#include <Algorithm/AStar/Navigation/NavigationGrid.h>

#include <Windows.h>
#include <algorithm>

namespace Craft
{
	DebugManager& DebugManager::Get()
	{
		static DebugManager instance;

		return instance;
	}

	void DebugManager::SetCameraController(CameraController* newCameraController)
	{
		cameraController = newCameraController;

		if (cameraController)
		{
			previousDeadZoneActive = cameraController->IsDeadZoneActive();
		}
		else
		{
			previousDeadZoneActive = false;
		}

		deadZoneExitTimer = 0.0f;
	}

	void DebugManager::Tick(float deltaTime)
	{
		// 토글 형식 디버그 Input
		if (Input::Get().GetKeyDown(VK_F3))
		{
			deadZoneDebugEnabled = !deadZoneDebugEnabled;

			if (!deadZoneDebugEnabled)
			{
				deadZoneExitTimer = 0.0f;
			}
		}

		// F4 : A* Debug
		if (Input::Get().GetKeyDown(VK_F4))
		{
			aStarDebugEnabled = !aStarDebugEnabled;
		}

		UpdateDeadZoneDebug(deltaTime);
	}

	void DebugManager::Draw()
	{
		DebugRenderer& debugRenderer = DebugRenderer::Get();

		// 이전 프레임 디버그 데이터 제거
		debugRenderer.BeginFrame();

		DrawDeadZoneDebug();

		// A*
		DrawAStarDebug();

		// Debug 렌더 명령을 실제 렌더러에게 전달
		debugRenderer.DebugFlush();
	}

	void DebugManager::SetAStarDebugData(const NavigationGrid* newNavigationGrid, const AStarPathFinder* newPathFinder)
	{
		navigationGrid = newNavigationGrid;
		aStarPathFinder = newPathFinder;
	}

	void DebugManager::UpdateDeadZoneDebug(float deltaTime)
	{
		// 예외 처리
		if (!cameraController)
		{
			return;
		}

		// 기존 데드존 이탈 시간 감소
		if (deadZoneExitTimer > 0.0f)
		{
			deadZoneExitTimer -= deltaTime;

			if (deadZoneExitTimer < 0.0f)
			{
				deadZoneExitTimer = 0.0f;
			}
		}

		const bool currentDeadZoneActive = cameraController->IsDeadZoneActive();

		if (deadZoneDebugEnabled)
		{
			// 데드존 이탈 감지
			if (previousDeadZoneActive && !currentDeadZoneActive)
			{
				deadZoneExitTimer = deadZoneExitDuration;
			}
		}

		previousDeadZoneActive = currentDeadZoneActive;
	}

	void DebugManager::DrawDeadZoneDebug()
	{
		if (!deadZoneDebugEnabled)
		{
			return;
		}

		if (!cameraController)
		{
			return;
		}

		const Craft::Vector2 viewportSize = Renderer::Get().GetCamera().GetViewportSize();

		const int deadZoneHalfWidth = static_cast<int>(cameraController->GetDeadZoneWidth());
		const int deadZoneHalfHeight = static_cast<int>(cameraController->GetDeadZoneHeight());

		const Craft::Vector2 deadZoneSize(deadZoneHalfWidth * 2 + 1, deadZoneHalfHeight * 2 + 1);

		const Craft::Vector2 deadZonePosition((viewportSize.x / 2) - deadZoneHalfWidth, (viewportSize.y / 2) - deadZoneHalfHeight);

		// 데드존 이탈 후
		if (deadZoneExitTimer > 0.0f)
		{
			DebugRenderer::Get().FillScreenRect(deadZonePosition, deadZoneSize, Craft::ColorRGB(190, 110, 110), 5);

			return;
		}

		// 데드존 활성 상태
		if (cameraController->IsDeadZoneActive())
		{
			DebugRenderer::Get().FillScreenRect(deadZonePosition, deadZoneSize, Craft::ColorRGB(120, 180, 120), 5);
		}
	}

	void DebugManager::DrawAStarDebug()
	{
		// A* Debug가 꺼져 있으면 그리지 않음
		if (!aStarDebugEnabled)
		{
			return;
		}

		// 필요한 데이터가 연결되지 않았다면 종료
		if (!navigationGrid || !aStarPathFinder)
		{
			return;
		}

		DebugRenderer& debugRenderer = DebugRenderer::Get();

		const int cellSize = navigationGrid->GetCellSize();

		// GridToWorld()는 Cell 중심을 반환하므로 FillWorldRect에서 사용할 왼쪽 위 좌표를 구하기 위해
		// Cell 크기의 절반을 사용
		const int halfCellSize = cellSize / 2;

		// Cell 전체를 덮지 않고 안쪽만 표시하기 위한 여백
		const int debugInset = 2;
		const Craft::Vector2 debugCellSize((std::max)(1, cellSize - debugInset * 2), (std::max)(1, cellSize - debugInset * 2));

		// Grid 좌표 하나를 Debug Cell로 출력하는 공통 함수
		auto drawGridCell = [&](const Craft::Vector2& gridPosition, const Craft::ColorRGB& color, int sortingOrder)
			{
				const Craft::Vector2F worldCenter = navigationGrid->GridToWorld(gridPosition);

				// GridToWorld()는 Cell 중심이므로 왼쪽 위 좌표로 변환
				const Craft::Vector2 cellTopLeft(static_cast<int>(worldCenter.x) - halfCellSize, static_cast<int>(worldCenter.y) - halfCellSize);

				// Cell 테두리와 약간 띄워서 내부만 표시
				const Craft::Vector2 debugPosition(cellTopLeft.x + debugInset, cellTopLeft.y + debugInset);

				debugRenderer.FillWorldRect(debugPosition, debugCellSize, color, sortingOrder);
			};

		// 1. 이동 불가능 Cell 표시
		for (int y = 0; y < navigationGrid->GetHeight(); ++y)
		{
			for (int x = 0; x < navigationGrid->GetWidth(); ++x)
			{
				// 이동 가능한 Cell은 건너뜀
				if (navigationGrid->IsWalkable(x, y))
				{
					continue;
				}

				// 장애물 Cell
				drawGridCell(Craft::Vector2(x, y), Craft::ColorRGB(150, 70, 70), 900);
			}
		}

		// 2. Open List 표시
		for (const Craft::Vector2& position : aStarPathFinder->GetOpenList())
		{
			drawGridCell(position, Craft::ColorRGB(90, 160, 90), 905);
		}

		// 3. Closed List 표시
		for (const Craft::Vector2& position : aStarPathFinder->GetClosedList())
		{
			drawGridCell(position, Craft::ColorRGB(80, 110, 170), 910);
		}

		// 4. 최종 Path 표시
		for (const Craft::Vector2& position : aStarPathFinder->GetLastPath())
		{
			drawGridCell(position, Craft::ColorRGB(230, 200, 70), 920);
		}
	}
}