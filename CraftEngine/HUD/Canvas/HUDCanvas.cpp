#include "HUDCanvas.h"

#include <algorithm>

namespace Craft
{
	void HUDCanvas::AddWidget(const std::shared_ptr<HUDWidget>& widget)
	{
		if (widget == nullptr)
		{
			return;
		}

		// 같은 Widget이 중복 등록되는 것 방지
		const auto found = std::find(widgets.begin(), widgets.end(), widget);

		if (found != widgets.end())
		{
			return;
		}

		widgets.push_back(widget);
	}

	void HUDCanvas::RemoveWidget(const std::shared_ptr<HUDWidget>& widget)
	{
		if (widget == nullptr)
		{
			return;
		}

		widgets.erase(std::remove(widgets.begin(), widgets.end(), widget), widgets.end());
	}

	void HUDCanvas::Clear()
	{
		widgets.clear();
	}

	void HUDCanvas::Draw()
	{
		if (!visible)
		{
			return;
		}

		// 각 Widget은 자기 내부에서 visible 여부를 다시 검사
		// 따라서 Canvas 전체가 보이더라도 특정 Widget만 숨기는 것도 가능
		for (const std::shared_ptr<HUDWidget>& widget : widgets)
		{
			if (widget == nullptr)
			{
				continue;
			}

			widget->Draw();
		}
	}
}