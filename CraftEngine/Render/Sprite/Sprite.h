#pragma once

#include <Core/Core.h>
#include <vector>
#include <string>
#include <initializer_list>

namespace Craft
{
	// 여러 줄의 문자열 이미지를 저장하는 클래스
	// 일반 문자 기반 배경 / HUD / 문자Sprite 등에 사용
	class CRAFT_API Sprite
	{
	public:
		Sprite() = default;

		// { ... , ... } 형태로 중괄호 안의 문자열 한번에 받음
		Sprite(std::initializer_list<std::string> lines);

		// push_back을 이용한 문자열 추가
		Sprite(const std::vector<std::string>& lines);

		// Sprite의 가장 긴 줄을 기준으로 너비 반환
		int GetWidth() const;

		// Sprite의 줄 개수로 높이 반환
		int GetHeight() const;

		// Sprite가 비어있는지 확인
		inline bool IsEmpty() const { return lines.empty(); }

		// 전체 문자열 데이터 반환
		inline const std::vector<std::string>& GetLines() const { return lines; }

	private:
		// Sprite를 구성하는 문자열 배열
		std::vector<std::string> lines;
	};
}