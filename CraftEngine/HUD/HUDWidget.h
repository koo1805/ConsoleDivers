#pragma once

#include <Core/Core.h>
#include <Math/Vector2.h>

namespace Craft
{
	// 모든 HUD 요소가 상속받는 기본 클래스
	// HUD는 Actor와 다르게 월드에 존재하지 않음
	// 따라서 Camera의 영향을 받지 않는 화면 좌표를 사용
	// ============================================================
	class CRAFT_API HUDWidget
	{
	public:
		HUDWidget() = default;

		HUDWidget(const Vector2& position, const Vector2& size = Vector2::Zero);

		virtual ~HUDWidget() = default;

	public:
		// HUD 그리기
		// 파생 HUD 요소가 각자 자신의 그리기 방식을 구현
		virtual void Draw() = 0;

	public:
		// 위치
		inline const Vector2& GetPosition() const { return position; }

		inline void SetPosition(const Vector2& newPosition) { position = newPosition; }

		// 크기
		inline const Vector2& GetSize() const { return size; }

		inline void SetSize(const Vector2& newSize) { size = newSize; }

		// HUD 표시 여부
		inline bool IsVisible() const { return visible; }

		inline void SetVisible(bool newVisible) { visible = newVisible; }

		// HUD끼리 겹칠 경우 그리기 우선순위
		inline int GetSortingOrder() const { return sortingOrder; }

		inline void SetSortingOrder(int newSortingOrder) { sortingOrder = newSortingOrder; }

	protected:
		// 화면 기준 위치
		Vector2 position = Vector2::Zero;

		// HUD 영역 크기
		Vector2 size = Vector2::Zero;

		// 표시 여부
		bool visible = true;

		// 일반 World Actor보다 높은 값을 기본값으로 사용
		// 이후 GameHUD의 모든 요소를 World보다 위에 그리기 위함
		int sortingOrder = 1000;
	};
}