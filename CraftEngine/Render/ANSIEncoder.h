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

		// 배경색 ANSI 코드 생성
		static std::string Background(const ColorRGB& color);

		// ANSI 색상 설정 초기화
		static std::string Reset();

		// 콘솔 커서 위치 관련
		// ----------------------------------------------------------
		// 콘솔 커서를 화면 왼쪽 위로 이동
		static std::string CursorHome();

		// 콘솔 커서를 지정된 위치로 이동
		static std::string CursorPosition(int x, int y);

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
	};
}
