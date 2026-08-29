#pragma once

#include <Math/Vector2.h>
#include <Render/Cell.h>
#include <vector>

namespace Craft
{
	// 이중 버퍼링 구현을 위한 화면 버퍼 클래스
	// 콘솔 핸들을 관리
	class ScreenBuffer
	{
	public:
		// 화면 크기를 받아 ScreenBuffer 생성
		ScreenBuffer(const Vector2& screenSize);
		~ScreenBuffer();

		// 버퍼의 모든 Cell을 기본 상태로 초기화
		void Clear();

		// 지정한 위치에 Cell을 저장
		void SetCell(const Vector2& position, const Cell& cell);

		// 지정한 위치의 Cell 반환
		Cell& GetCell(const Vector2& position);
		const Cell& GetCell(const Vector2& position) const;

		// 현재 버퍼의 크기 반환
		inline const Vector2& GetSize() const { return size; }

		// 내부 Cell 배열을 읽기 전용으로 반환
		// Renderer::Present()처럼 이미 유효 범위가 보장된 순회에서 사용
		inline const std::vector<Cell>& GetCells() const { return cells; }

	private:
		// 2차원 좌표를 vector에서 사용할 1차원 index로 변환
		int GetIndex(const Vector2& position) const;

		// 좌표가 화면 범위안에 있는지 확인
		bool IsValidPosition(const Vector2& position) const;

	private:
		// 화면 각 위치에 출력될 최종 Cell데이터 - 실제 저장은 1차원 배열 방식
		std::vector<Cell> cells;

		// 화면 크기
		Vector2 size;
	};
}