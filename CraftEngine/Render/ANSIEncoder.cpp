#include "ANSIEncoder.h"
#include <Render/Cell.h>
#include <charconv>

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
		std::string result;

		AppendForeground(result, color);

		return result;
	}

	void ANSIEncoder::AppendForeground(std::string& output, const ColorRGB& color)
	{
		// ANSI 24bit 전경색 시작 코드
		output += "\x1b[38;2;";

		// R
		AppendInteger(output, static_cast<int>(color.r));
		output += ';';

		// G
		AppendInteger(output, static_cast<int>(color.g));
		output += ';';

		// B
		AppendInteger(output, static_cast<int>(color.b));

		// ANSI 명령 종료
		output += 'm';

	}

	// 24bit ANSI 배경색 코드 생성
	// 형식: ESC[48;2;R;G;Bm
	std::string ANSIEncoder::Background(const ColorRGB& color)
	{
		std::string result;

		AppendBackground(result, color);

		return result;
	}

	void ANSIEncoder::AppendBackground(std::string& output, const ColorRGB& color)
	{
		// ANSI 24bit 전경색 시작 코드
		output += "\x1b[48;2;";

		// R
		AppendInteger(output, static_cast<int>(color.r));
		output += ';';

		// G
		AppendInteger(output, static_cast<int>(color.g));
		output += ';';

		// B
		AppendInteger(output, static_cast<int>(color.b));

		// ANSI 명령 종료
		output += 'm';
	}

	void ANSIEncoder::AppendColors(std::string& output, const ColorRGB& foreground, const ColorRGB& background)
	{
		// ANSI 24-bit 전경색 + 배경색
		// 예: \x1b[38;2;255;0;0;48;2;0;0;0m
		output += "\x1b[38;2;";

		// 전경색 R
		AppendInteger(output, static_cast<int>(foreground.r));
		output += ';';

		// 전경색 G
		AppendInteger(output, static_cast<int>(foreground.g));
		output += ';';

		// 전경색 B
		AppendInteger(output, static_cast<int>(foreground.b));

		// 배경색 시작
		output += ";48;2;";

		// 배경색 R
		AppendInteger(output, static_cast<int>(background.r));
		output += ';';

		// 배경색 G
		AppendInteger(output, static_cast<int>(background.g));
		output += ';';

		// 배경색 B
		AppendInteger(output, static_cast<int>(background.b));
		output += 'm';
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
		std::string result;
		AppendCursorPosition(result, x, y);

		return result;
	}

	void ANSIEncoder::AppendCursorPosition(std::string& output, int x, int y)
	{
		output += "\x1b[";
		AppendInteger(output, y);
		output += ';';
		AppendInteger(output, x);
		output += 'H';
	}

	void ANSIEncoder::AppendCursorForward(std::string& output, int distance)
	{
		// 이동할 필요가 없으면 아무것도 추가하지 않음
		if (distance <= 0)
		{
			return;
		}

		// ANSI Cursor Forward -> ESC[nC
		output += "\x1b[";
		// ANSI의 기본 이동 거리는 1이므로 distance == 1이면 숫자를 생략할 수 있음
		if (distance > 1)
		{
			AppendInteger(output, distance);
		}
		output += 'C';
	}

	void ANSIEncoder::AppendCursorDown(std::string& output, int distance)
	{
		// 이동할 필요가 없으면 출력하지 않음
		if (distance <= 0)
		{
			return;
		}

		// ANSI Cursor Down -> ESC[nB
		output += "\x1b[";
		// ANSI의 기본 이동 거리는 1이므로 distance == 1이면 숫자를 생략할 수 있음
		if (distance > 1)
		{
			AppendInteger(output, distance);
		}
		output += 'B';
	}

	void ANSIEncoder::AppendCursorColumn(std::string & output, int column)
	{
		// ANSI Cursor Horizontal Absolute -> ESC[nG
		output += "\x1b[";
		AppendInteger(output, column);
		output += 'G';
	}

	void ANSIEncoder::AppendCursorNextLine(std::string & output, int distance)
	{
		if (distance <= 0)
		{
			return;
		}

		// ANSI Cursor Next Line -> ESC[nE
		// 지정된 행만큼 아래로 이동하면서 커서를 해당 행의 첫 번째 열로 이동
		output += "\x1b[";
		// ANSI의 기본 이동 거리는 1이므로 distance == 1이면 숫자를 생략할 수 있음
		if (distance > 1)
		{
			AppendInteger(output, distance);
		}
		output += 'E';
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

	void ANSIEncoder::AppendInteger(std::string& output, int value)
	{
		// int 범위를 충분히 저장할 수 있는 임시 문자 배열
		char buffer[16];

		// 숫자를 문자로 바로 변환
		// std::to_string()처럼 임시 std::string을 만들지 않음
		const auto result = std::to_chars(buffer, buffer + sizeof(buffer), value);

		// 변환된 문자만 기존 output에 바로 추가
		output.append(buffer, result.ptr);
	}
}