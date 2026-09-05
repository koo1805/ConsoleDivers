#include "DebugManager.h"
#include <Actor/Actor.h>
#include <Camera/Camera.h>
#include <Camera/Controller/CameraController.h>
#include <Render/Renderer.h>
#include <Render/Debugging/DebugRenderer.h>
#include <Input/Input.h>
#include <Algorithm/AStar/AStarPathFinder.h>
#include <Algorithm/AStar/Navigation/NavigationGrid.h>
#include <Algorithm/QuadTree/QuadTree.h>
#include <Algorithm/QuadTree/QuadTreeNode.h>

#include <Windows.h>
#include <algorithm>
#include <cmath>

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

		if (Input::Get().GetKeyDown(VK_F5))
		{
			quadTreeDebugEnabled = !quadTreeDebugEnabled;
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

		// QuadTree
		DrawQuadTreeDebug();

		// QuadTree Query 영역 및 결과
		DrawQuadTreeQueryDebug();

		// Debug 렌더 명령을 실제 렌더러에게 전달
		debugRenderer.DebugFlush();
	}

	void DebugManager::SetAStarDebugData(const NavigationGrid* newNavigationGrid, const AStarPathFinder* newPathFinder)
	{
		navigationGrid = newNavigationGrid;
		aStarPathFinder = newPathFinder;
	}

	void DebugManager::SetQuadTreeDebugData(const QuadTree* newQuadTree)
	{
		// DebugManager는 시각화를 위해 참조만 보관
		quadTree = newQuadTree;
	}

	void DebugManager::SetQuadTreeQueryDebugData(float x, float y, float width, float height, const std::vector<std::shared_ptr<Actor>>& queryResults)
	{
		// 현재 프레임 Query 영역 저장
		quadTreeQueryX = x;
		quadTreeQueryY = y;

		quadTreeQueryWidth = width;
		quadTreeQueryHeight = height;

		// 이전 Query 결과 제거
		quadTreeQueryResults.clear();

		// Actor 소유권은 Level이 가지고 있으므로 DebugManager에서는 weak_ptr로만 보관
		quadTreeQueryResults.reserve(queryResults.size());

		for (const std::shared_ptr<Actor>& actor : queryResults)
		{
			if (!actor)
			{
				continue;
			}

			quadTreeQueryResults.emplace_back(actor);
		}
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

	void DebugManager::DrawQuadTreeDebug()
	{
		// QuadTree 시각화가 꺼져 있으면 아무것도 하지 않음
		if (!quadTreeDebugEnabled)
		{
			return;
		}

		// GameLevel에서 QuadTree가 연결되지 않은 경우
		if (!quadTree)
		{
			return;
		}

		// QuadTree의 시작점인 Root를 얻음
		const QuadTreeNode* root = quadTree->GetRoot();

		if (!root)
		{
			return;
		}

		// Root부터 재귀적으로 모든 Node 출력
		DrawQuadTreeNodeDebug(root);
	}

	void DebugManager::DrawQuadTreeNodeDebug(const QuadTreeNode * node)
	{
		if (!node)
		{
			return;
		}

		DebugRenderer& debugRenderer = DebugRenderer::Get();

		const QuadTreeBounds& bounds = node->GetBounds();

		const int left = static_cast<int>(std::floor(bounds.x));
		const int right = static_cast<int>(std::ceil(bounds.GetMaxX()));

		const int top = static_cast<int>(std::floor(bounds.y));
		const int bottom = static_cast<int>(std::ceil(bounds.GetMaxY()));

		const Vector2 position(left, top);
		const Vector2 size((std::max)(1, right - left), (std::max)(1, bottom - top));

		ColorRGB color;

		switch (node->GetDepth())
		{
		case 0:
			color = ColorRGB(230, 230, 230);
			break;

		case 1:
			color = ColorRGB(220, 170, 80);
			break;

		case 2:
			color = ColorRGB(100, 190, 220);
			break;

		case 3:
			color = ColorRGB(130, 210, 130);
			break;

		default:
			color = ColorRGB(200, 120, 200);
			break;
		}

		// 실제 QuadTree Node 영역 출력
		// World 좌표 기준이므로 카메라가 움직여도 월드의 실제 분할 위치에 고정되어 보임
		debugRenderer.DrawWorldRect(position, size, color, 940 + node->GetDepth());

		// 자식이 없다면 더 내려갈 필요가 없음
		if (!node->IsDivided())
		{
			return;
		}

		// 현재 Node가 분할되어 있다면 자식 4개를 다시 같은 방식으로 출력
		// 재귀를 통해 최대 깊이까지 자동으로 내려감
		DrawQuadTreeNodeDebug(node->GetTopLeft());
		DrawQuadTreeNodeDebug(node->GetTopRight());
		DrawQuadTreeNodeDebug(node->GetBottomLeft());
		DrawQuadTreeNodeDebug(node->GetBottomRight());
	}

	void DebugManager::DrawQuadTreeQueryDebug()
	{
		if (!quadTreeDebugEnabled)
		{
			return;
		}

		DebugRenderer& debugRenderer = DebugRenderer::Get();

		// QueryBounds 변환
		const int left = static_cast<int>(std::floor(quadTreeQueryX));
		const int right = static_cast<int>(std::ceil(quadTreeQueryX + quadTreeQueryWidth));

		const int top = static_cast<int>(std::floor(quadTreeQueryY));
		const int bottom = static_cast<int>(std::ceil(quadTreeQueryY + quadTreeQueryHeight));

		const Vector2 queryPosition(left, top);

		const Vector2 querySize((std::max)(1, right - left), (std::max)(1, bottom - top));

		// 현재 Query 영역
		debugRenderer.DrawWorldRect(queryPosition, querySize, ColorRGB(240, 220, 80), 970);

		// Query 결과 Actor 강조
		for (const std::weak_ptr<Actor>& weakActor : quadTreeQueryResults)
		{
			const std::shared_ptr<Actor> actor = weakActor.lock();

			// 이미 제거된 Actor
			if (!actor)
			{
				continue;
			}

			// 비활성 Actor
			if (!actor->IsActive())
			{
				continue;
			}

			const Vector2F actorPosition = actor->GetPosition();

			const Vector2 actorDebugPosition(static_cast<int>(std::floor(actorPosition.x)), static_cast<int>(std::floor(actorPosition.y)));

			const Vector2 actorDebugSize((std::max)(1, actor->GetWidth()), (std::max)(1, actor->GetHeight()));

			// Query 결과에 포함된 Actor는 // 밝은 빨간색 Bounding Box로 표시
			debugRenderer.DrawWorldRect(actorDebugPosition, actorDebugSize, ColorRGB(255, 80, 80), 980);
		}
	}
}