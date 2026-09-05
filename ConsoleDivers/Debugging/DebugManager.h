#pragma once

// Craft 외 클래스 전방 선언
class NavigationGrid;
class AStarPathFinder;

namespace Craft
{
	// 전방 선언
	class CameraController;

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
	};
}