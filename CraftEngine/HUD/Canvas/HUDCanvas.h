#pragma once

#include <Core/Core.h>
#include <HUD/HUDWidget.h>

#include <memory>
#include <vector>

namespace Craft
{
	// HUD Widget들을 모아서 관리하는 Canvas
	// 실제 UI 디자인은 하지 않음
	// ============================================================
	class CRAFT_API HUDCanvas
	{
	public:
		HUDCanvas() = default;
		~HUDCanvas() = default;

	public:
		// Widget 추가
		void AddWidget(const std::shared_ptr<HUDWidget>& widget);

		// Widget 제거
		void RemoveWidget(const std::shared_ptr<HUDWidget>& widget);

		// 모든 Widget 제거
		void Clear();

		// Canvas에 등록된 HUD를 전부 그린다.
		void Draw();

		// Canvas 전체 표시 여부
		inline bool IsVisible() const { return visible; }

		inline void SetVisible(bool newVisible) { visible = newVisible; }

	private:
		std::vector<std::shared_ptr<HUDWidget>> widgets;

		bool visible = true;
	};
}