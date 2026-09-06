#include "HUDManager.h"

#include <algorithm>

namespace Craft
{
	HUDManager& HUDManager::Get()
	{
		static HUDManager instance;
		return instance;
	}

	void HUDManager::AddCanvas(const std::shared_ptr<HUDCanvas>& canvas)
	{
		if (canvas == nullptr)
		{
			return;
		}

		const auto found = std::find(canvases.begin(), canvases.end(), canvas);

		if (found != canvases.end())
		{
			return;
		}

		canvases.push_back(canvas);
	}

	void HUDManager::RemoveCanvas(const std::shared_ptr<HUDCanvas>& canvas)
	{
		if (canvas == nullptr)
		{
			return;
		}

		canvases.erase(std::remove(canvases.begin(), canvases.end(), canvas), canvases.end());
	}

	void HUDManager::Clear()
	{
		canvases.clear();
	}

	void HUDManager::Draw()
	{
		for (const std::shared_ptr<HUDCanvas>& canvas : canvases)
		{
			if (canvas == nullptr)
			{
				continue;
			}

			canvas->Draw();
		}
	}
}