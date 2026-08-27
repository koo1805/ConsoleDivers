#include "ScreenBuffer.h"
#include <cassert>

namespace Craft
{
	ScreenBuffer::ScreenBuffer(const Vector2& screenSize)
		: size(screenSize),
		cells(static_cast<size_t>(screenSize.x * screenSize.y))
	{ }

	ScreenBuffer::~ScreenBuffer()
	{ }

	// Cell의 기본 생성값을 빈 화면 상태로 사용
	void ScreenBuffer::Clear()
	{
		const Cell emptyCell;

		// 모든 Cell을 기본 상태로 초기화
		for (Cell& cell : cells)
		{
			cell = emptyCell;
		}
	}

	void ScreenBuffer::SetCell(const Vector2& position, const Cell& cell)
	{
		// 화면 밖의 좌표라면 그리지 않음
		// Sprite가 화면 밖으로 일부 벗어났을 때 - 잘못된 메모리에 접근하는 것을 방지
		if (!IsValidPosition(position))
		{
			return;
		}

		// 2차원 좌표를 1차원 index로 변환하여 저장
		cells[GetIndex(position)] = cell;
	}

	Cell& ScreenBuffer::GetCell(const Vector2& position)
	{
		// 잘못된 좌표가 들어오면 Debug에서 바로 확인
		assert(IsValidPosition(position));

		return cells[GetIndex(position)];
	}

	const Cell& ScreenBuffer::GetCell(const Vector2& position) const
	{
		// 잘못된 좌표가 들어오면 Debug에서 바로 확인
		assert(IsValidPosition(position));

		return cells[GetIndex(position)];
	}

	// ScreenBuffer는 논리적으로 2차원이지만 실제 데이터는 1차원 vector에 연속해서 저장
	int ScreenBuffer::GetIndex(const Vector2& position) const
	{
		return position.y * size.x + position.x;
	}

	// 현재 위치 값이 유효한지 검사
	bool ScreenBuffer::IsValidPosition(const Vector2& position) const
	{
		return (position.x >= 0)
			&& (position.x < size.x)
			&& (position.y >= 0)
			&& (position.y < size.y);
	}
}