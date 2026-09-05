#pragma once

#include <memory>
#include <vector>

// Craft 외 클래스 전방 선언
class NavigationGrid;
class AStarPathFinder;

namespace Craft
{
	// 전방 선언
	class CameraController;
	class QuadTree;
	class QuadTreeNode;
	class Actor;

	class DebugManager
	{
	public:
		static DebugManager& Get();

	public:
		// 디버그에서 사용할 카메라 설정
		void SetCameraController(CameraController* newCameraController);

		// 매 프레임 Debug 상태 처리
		void Tick(float deltaTime);

		// Debug 렌더링
		void Draw();

		// A* 디버그에서 사용할 Navigation / PathFinder 연결
		void SetAStarDebugData(const NavigationGrid* newNavigationGrid, const AStarPathFinder* newPathFinder);

		// QuadTree 시각화에서 사용할 QuadTree 연결
		void SetQuadTreeDebugData(const QuadTree* newQuadTree);

		// QuadTree Query 디버그 데이터 설정
		void SetQuadTreeQueryDebugData(float x, float y, float width, float height, const std::vector<std::shared_ptr<Actor>>& queryResults);

	private:
		DebugManager() = default;
		~DebugManager() = default;

		// 싱글톤 - 프로그램 전체에서 객체를 딱 하나만 만들어서 공유해서 쓰는 패턴
		// 복사 생성 금지
		DebugManager(const DebugManager&) = delete;

		// 복사 대입 금지
		DebugManager& operator=(const DebugManager&) = delete;

	private:
		// 데드존 디버그 처리
		void UpdateDeadZoneDebug(float deltaTime);

		// 데드존 디버그 그리기
		void DrawDeadZoneDebug();

		// A* 디버그 그리기
		void DrawAStarDebug();

		// QuadTree 디버그 그리기
		void DrawQuadTreeDebug();

		// 하나의 QuadTreeNode와 자식들을 재귀적으로 그리기
		void DrawQuadTreeNodeDebug(const QuadTreeNode* node);

		// QuadTree Query 영역 및 검색 결과 표시
		void DrawQuadTreeQueryDebug();

	private:
		CameraController* cameraController = nullptr;

		// 데드존 디버그 활성 여부
		bool deadZoneDebugEnabled = false;

		// 이전 프레임 데드존 상태
		bool previousDeadZoneActive = false;

		// 데드존 이탈 색상 표시 타이머
		float deadZoneExitTimer = 0.0f;

		// 색상 유지 시간
		float deadZoneExitDuration = 0.15f;

		// GameLevel이 소유하고 있는 NavigationGrid를 참조
		const NavigationGrid* navigationGrid = nullptr;

		// GameLevel이 소유하고 있는 AStarPathFinder를 참조
		const AStarPathFinder* aStarPathFinder = nullptr;

		// F4로 A* 시각화 ON / OFF
		bool aStarDebugEnabled = false;

		// GameLevel이 소유하고 있는 QuadTree를 참조
		const QuadTree* quadTree = nullptr;

		// F5로 QuadTree 시각화 ON / OFF
		bool quadTreeDebugEnabled = false;

		// Query 영역 왼쪽 위 위치
		float quadTreeQueryX = 0.0f;
		float quadTreeQueryY = 0.0f;

		// Query 영역 크기
		float quadTreeQueryWidth = 0.0f;
		float quadTreeQueryHeight = 0.0f;

		// 현재 Query 결과 Actor | DebugManager가 Actor 수명을 연장하지 않도록 weak_ptr 사용
		std::vector<std::weak_ptr<Actor>> quadTreeQueryResults;
	};
}