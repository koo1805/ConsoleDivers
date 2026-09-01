#include "ConsoleContext.h"
#include <Render/ANSIEncoder.h>
#include <algorithm>
#include <iostream>

namespace Craft
{
	ConsoleContext::ConsoleContext()
	{
		Initialized();
	}

	ConsoleContext::~ConsoleContext()
	{
		Restore();
	}

	void ConsoleContext::Resize(const Vector2& screenSize)
	{
		// ConsoleContext 초기화에 실패한 상태라면 콘솔 크기 변경을 시도하지 않음
		if (!initialized)
		{
			return;
		}

		// 원하는 화면 크기
		// Windows Console API는 SHORT 타입을 사용하므로 Vector2 값을 SHORT로 변환
		SHORT targetWidth = static_cast<SHORT>(screenSize.x);

		SHORT targetHeight = static_cast<SHORT>(screenSize.y);

		// 너무 작은 값 방지
		targetWidth = std::max<SHORT>(1, targetWidth);
		targetHeight = std::max<SHORT>(1, targetHeight);

		// 현재 콘솔 정보 가져오기
		CONSOLE_SCREEN_BUFFER_INFO consoleInfo = {};

		if (!GetConsoleScreenBufferInfo(outputHandle, &consoleInfo))
		{
			return;
		}


		// ----------------------------------------------------
		// Windows Console의 크기 변경에는 제약이 있다.
		//
		// Window 영역은 ScreenBuffer보다 클 수 없다.
		//
		// 따라서 크기를 줄이는 경우에는
		//
		// 1. Window를 먼저 작게 만든다.
		// 2. ScreenBuffer를 줄인다.
		//
		// 반대로 크기를 키우는 경우에는
		//
		// 1. ScreenBuffer를 먼저 키운다.
		// 2. Window를 키운다.
		//
		// 이 순서를 지켜야 Resize 실패 가능성을 줄일 수 있다.
		// ----------------------------------------------------


		// 현재 콘솔 창 크기 계산
		SHORT currentWindowWidth = consoleInfo.srWindow.Right - consoleInfo.srWindow.Left + 1;

		SHORT currentWindowHeight = consoleInfo.srWindow.Bottom - consoleInfo.srWindow.Top + 1;

		// 크기가 줄어드는 경우
		if (targetWidth < currentWindowWidth || targetHeight < currentWindowHeight)
		{
			SMALL_RECT temporaryWindow = {};

			temporaryWindow.Left = 0;
			temporaryWindow.Top = 0;

			temporaryWindow.Right = std::min<SHORT>(currentWindowWidth, targetWidth) - 1;

			temporaryWindow.Bottom = std::min<SHORT>(currentWindowHeight, targetHeight) - 1;

			// 먼저 Window 영역을 줄인다.
			SetConsoleWindowInfo(outputHandle, TRUE, &temporaryWindow);
		}

		// ScreenBuffer 크기 변경
		COORD bufferSize = {};

		bufferSize.X = targetWidth;
		bufferSize.Y = targetHeight;

		SetConsoleScreenBufferSize(outputHandle, bufferSize);


		// ----------------------------------------------------
		// 최종 Window 영역 설정
		//
		// SMALL_RECT는 크기가 아니라
		// 마지막 Cell의 좌표를 사용한다.
		//
		// 예:
		//
		// 160 x 45
		//
		// Right  = 159
		// Bottom = 44
		// ----------------------------------------------------

		SMALL_RECT windowRect = {};

		windowRect.Left = 0;
		windowRect.Top = 0;

		windowRect.Right = targetWidth - 1;

		windowRect.Bottom = targetHeight - 1;

		SetConsoleWindowInfo(outputHandle, TRUE, &windowRect);
	}

	// 현재 실제 표시 영역 가져오기
	Vector2 ConsoleContext::GetViewportSize() const
	{
		if (!initialized)
		{
			return Vector2();
		}

		CONSOLE_SCREEN_BUFFER_INFO consoleInfo = {};

		if (!GetConsoleScreenBufferInfo(outputHandle, &consoleInfo))
		{
			return Vector2();
		}

		const int width = consoleInfo.srWindow.Right - consoleInfo.srWindow.Left + 1;
		const int height = consoleInfo.srWindow.Bottom - consoleInfo.srWindow.Top + 1;

		return Vector2(width, height);
	}

	bool ConsoleContext::CanFitViewport(const Vector2& screenSize) const
	{
		const Vector2 viewportSize = GetViewportSize();

		return viewportSize.x >= screenSize.x && viewportSize.y >= screenSize.y;
	}

	void ConsoleContext::Initialized()
	{
		// Windows의 표준 출력 Handle
		outputHandle = GetStdHandle(STD_OUTPUT_HANDLE);

		// 예외 처리
		if (outputHandle == INVALID_HANDLE_VALUE || outputHandle == nullptr)
		{
			return;
		}

		// 현재 콘솔 모드를 가져옴
		// defaultConsoleMode에 기존 값 저장 - 종료 시 복구하기 위한 작업
		if (!GetConsoleMode(outputHandle, &defaultConsoleMode))
		{
			return;
		}

		// 기존 consoleMode 복사
		DWORD consoleMode = defaultConsoleMode;

		// ANSI Escape Sequence를 windows 콘솔에서 사용가능하도록 함
		// "\x1b[38;2;255;0;0m"	>  24bit ANSI 명령을 실제 색상 변경 명령으로 처리함
		consoleMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;

		// 변경된 콘솔 모드 적용
		if (!SetConsoleMode(outputHandle, consoleMode))
		{
			return;
		}

		// 여기까지 진행시 정상적으로 콘솔 초기화됨
		initialized = true;

		// 자동 줄바꿈 비활성화
		// CursorPosition으로 위치 직접 지정 > 자동 줄바꿈시 실제 좌표와 어긋날 수 있음
		std::cout << ANSIEncoder::DisableAutoWrap();

		// 콘솔 커서 숨김
		std::cout << ANSIEncoder::HideCursor();

		// ANSI설정을 즉시 콘솔에 반영
		std::cout.flush();
	}

	void ConsoleContext::Restore()
	{
		// 초기화가 정상적으로 끝나지 않으면 복원할 콘솔 모드가 없으므로 종료
		if (!initialized)
		{
			return;
		}

		// 출력상태 초기화
		std::cout << ANSIEncoder::Reset();

		// 자동 줄바꿈 다시 활성화
		std::cout << ANSIEncoder::EnableAutoWrap();

		// 콘솔 커서 표시
		std::cout << ANSIEncoder::ShowCursor();

		// ANSI설정을 즉시 콘솔에 반영
		std::cout.flush();

		// 원래 콘솔 모드로 복원
		SetConsoleMode(outputHandle, defaultConsoleMode);
	}
}