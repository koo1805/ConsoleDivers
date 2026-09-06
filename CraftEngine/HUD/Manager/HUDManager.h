#pragma once

#include <Core/Core.h>
#include <HUD/Canvas/HUDCanvas.h>

#include <memory>
#include <vector>

namespace Craft
{
	// 엔진 전체 HUDCanvas 관리
	// 여러 HUDCanvas를 등록하고 한 번에 Draw
	// 예:GameHUDCanvas / PauseHUDCanvas / DebugHUDCanvas
	// ============================================================
	class CRAFT_API HUDManager
	{
	public:
		static HUDManager& Get();

	private:
		HUDManager() = default;
		~HUDManager() = default;

		HUDManager(const HUDManager&) = delete;
		HUDManager& operator=(const HUDManager&) = delete;

	public:
		void AddCanvas(const std::shared_ptr<HUDCanvas>& canvas);

		void RemoveCanvas(const std::shared_ptr<HUDCanvas>& canvas);

		void Clear();

		void Draw();

	private:
		std::vector<std::shared_ptr<HUDCanvas>> canvases;
	};
}