#pragma once

#include <memory>

class Player;

namespace Craft
{
	class HUDCanvas;
	class HUDProgressBar;
}

class WeaponGaugeHUD
{
public:
	WeaponGaugeHUD() = default;
	~WeaponGaugeHUD() = default;

public:
	void Initialize(const std::shared_ptr<Craft::HUDCanvas>& canvas, const std::shared_ptr<Player>& player);
	void Update();

private:
	std::weak_ptr<Player> player;

	std::shared_ptr<Craft::HUDProgressBar> actionBar;

	static constexpr int BarWidth = 9;
	static constexpr int BarHeight = 1;
	static constexpr int PlayerBottomOffset = 16;
};