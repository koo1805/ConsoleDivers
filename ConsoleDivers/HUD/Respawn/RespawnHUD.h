#pragma once

#include <HUD/HUDLayoutContext.h>

#include <array>
#include <memory>

namespace Craft
{
	class HUDCanvas;
	class HUDNumber;
	class PixelSprite;
}

class RespawnHUD
{
public:
	RespawnHUD() = default;
	~RespawnHUD() = default;

public:
	void Initialize(const std::shared_ptr<Craft::HUDCanvas>& canvas, const std::array<const Craft::PixelSprite*, 10>& digitSprites);

	// 화면 위치에 따른 계산
	void UpdateLayout(const HUDLayoutContext& context);

	// HUD표시
	void Show();

	// HUD 숨김
	void Hide();

	// 남은 시간
	void SetRemainingTime(float remainingTime);

	inline bool IsVisible() const { return isVisible; }

private:
	std::shared_ptr<Craft::HUDNumber> countNumber;

	bool isVisible = false;

	static constexpr int DigitWidth = 3;
	static constexpr int DigitHeight = 3;

	static constexpr int VerticalOffset = -3;
	static constexpr int sortingOrder = 1100;
};