#pragma once

#include <Core/Core.h>
#include <Render/Cell.h>
#include <vector>

namespace Craft
{
	// 각 Cell마다 문자 / 전경색 / 배경색을 저장하는 Sprite
	// 24bit RGB Sprite에 사용
	class CRAFT_API PixelSprite
	{
	public:
		PixelSprite() = default;

		// 지정한 크기의 PixelSprite생성
		PixelSprite(int width, int height);

	public:
		// 지정한 위치의 Cell을 설정
		void SetCell(int x, int y, const Cell& cell);

		// 지정한 위치의 Cell을 반환
		const Cell& GetCell(int x, int y) const;

		// Sprite의 너비 반환
		inline int GetWidth() const { return width; }

		// Sprite의 높이 반환
		inline int GetHeight() const { return height; }

		// Sprite가 비어있는지 확인
		inline bool IsEmpty() const { return cells.empty(); }

		// 전체 Cell데이터 반환
		inline const std::vector<Cell>& GetCells() const { return cells; }

	private:
		// 너비
		int width = 0;

		// 높이
		int height = 0;

		// Sprite를 구성하는 Cell배열
		std::vector<Cell> cells;
	};
}