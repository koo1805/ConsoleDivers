#include "ANSIEncoder.h"
#include <Render/Cell.h>

namespace Craft
{
	// Cell 하나를 ANSI 문자열로 반환
	std::string ANSIEncoder::Encode(const Cell& cell)
	{
		std::string result;

		// 전경색 설정
		result += Foreground(cell.foreground);

		// 배경색 설정
		result += Background(cell.background);

		// 실제 출력할 문자 추가
		result += cell.character;

		return result;
	}

	// 24bit ANSI 전경색 코드 생성
	// 형식: ESC[38;2;R;G;Bm
	std::string ANSIEncoder::Foreground(const ColorRGB& color)
	{
		return "\x1b[38;2;"
			+ std::to_string(static_cast<int>(color.r)) + ";"
			+ std::to_string(static_cast<int>(color.g)) + ";"
			+ std::to_string(static_cast<int>(color.b)) + "m";
	}

	// 24bit ANSI 배경색 코드 생성
	// 형식: ESC[48;2;R;G;Bm
	std::string ANSIEncoder::Background(const ColorRGB& color)
	{
		return "\x1b[48;2;"
			+ std::to_string(static_cast<int>(color.r)) + ";"
			+ std::to_string(static_cast<int>(color.g)) + ";"
			+ std::to_string(static_cast<int>(color.b)) + "m";
	}

	// ANSI 색상 및 출력 속성 초기화
	std::string ANSIEncoder::Reset()
	{
		return "\x1b[0m";
	}

	// 콘솔 커서를 화면 좌상단으로 이동
	std::string ANSIEncoder::CursorHome()
	{
		return "\x1b[H";
	}

	// 콘솔 커서를 지정된 위치로 이동
	// ANSI 문자열은 (y, x)로 기록함
	std::string ANSIEncoder::CursorPosition(int x, int y)
	{
		return "\x1b[" + std::to_string(y) + ";" + std::to_string(x) + "H";
	}

	// 콘솔 커서 숨김 처리
	std::string ANSIEncoder::HideCursor()
	{
		return "\x1b[?25l";
	}

	// 콘솔 커서 표시 처리
	std::string ANSIEncoder::ShowCursor()
	{
		return "\x1b[?25h";
	}

	// 자동 줄바꿈 비활성화 처리
	std::string ANSIEncoder::DisableAutoWrap()
	{
		return "\x1b[?7l";
	}

	// 자동 줄바꿈 활성화 처리
	std::string ANSIEncoder::EnableAutoWrap()
	{
		return "\x1b[?7h";
	}
}