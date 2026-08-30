#pragma once

#include <Math/Vector2.h>
#include <Windows.h>

namespace Craft
{
	// Window 콘솔의 출력 환경을 관리하는 클래스
	class ConsoleContext
	{
	public:
		ConsoleContext();
		~ConsoleContext();

	public:
		void Resize(const Vector2& screenSize);

		// 현재 실제 표시 가능한 Window 영역 크기 반환
		Vector2 GetViewportSize() const;

		// 현재 Window가 요청한 화면 크기를 담을 수 있는지 확인
		bool CanFitViewport(const Vector2& screenSize) const;

	private:
		// 콘솔 화면 초기화 함수
		void Initialized();

		// 프로그램 종료시 기존 Console Mode 복원 함수
		void Restore();

	private:
		// 표준 출력 콘솔 Handle
		HANDLE outputHandle = INVALID_HANDLE_VALUE;

		// ConsoleContext가 변경하기 전의 원래 Console Mode
		DWORD defaultConsoleMode = 0;

		// Console Mode를 정상적으로 저장했는지 확인하는 값
		// 초기화에 실패한 상태에서 Restore()가 호출되는 것을 방지
		bool initialized = false;
	};
}