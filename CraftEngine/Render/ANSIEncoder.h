#pragma once

#include <Core/Core.h>
#include <Math/ColorRGB.h>
#include <string>

namespace Craft
{
	// 전방 선언
	struct Cell;

	// Cell 데이터를 24bit ANSI 문자열로 변환하는 클래스
	// ANSI Escape Sequence를 생성하는 클래스
	class CRAFT_API ANSIEncoder
	{
	public:
		// Cell 색상 관련
		// ----------------------------------------------------------
		// Cell 하나를 ANSI 문자열로 변환
		static std::string Encode(const Cell& cell);

		// 전경색 ANSI 코드 생성
		static std::string Foreground(const ColorRGB& color);

		static void AppendForeground(std::string& output, const ColorRGB& color);

		// 배경색 ANSI 코드 생성
		static std::string Background(const ColorRGB& color);

		static void AppendBackground(std::string& output, const ColorRGB& color);

		// 전경색과 배경색을 하나의 ANSI SGR 코드로 추가
		static void AppendColors(std::string& output, const ColorRGB& foreground, const ColorRGB& background);

		// ANSI 색상 설정 초기화
		static std::string Reset();

		// 콘솔 커서 위치 관련
		// ----------------------------------------------------------
		// 콘솔 커서를 화면 왼쪽 위로 이동
		static std::string CursorHome();

		// 콘솔 커서를 지정된 위치로 이동
		static std::string CursorPosition(int x, int y);

		// 커서 이동 ANSI 코드를 임시 문자열 생성 없이 기존 output에 바로 추가
		static void AppendCursorPosition(std::string& output, int x, int y);

		// 현재 위치에서 오른쪽으로 지정된 칸만큼 커서 이동  -> ANSI: ESC[nC
		static void AppendCursorForward(std::string& output, int distance);

		// 콘솔 커서 표시 관련
		// ----------------------------------------------------------
		// 콘솔 커서 숨심
		static std::string HideCursor();

		// 콘솔 커서 표시
		static std::string ShowCursor();

		// 자동 줄바꿈 관련
		// ----------------------------------------------------------
		// 자동 줄바꿈 비활성화
		static std::string DisableAutoWrap();

		// 자동 줄바꿈 활성화
		static std::string EnableAutoWrap();

	private:
		// 정수를 임시 std::string 생성 없이 기존 output 문자열에 바로 추가
		static void AppendInteger(std::string& output, int value);
	};
}
