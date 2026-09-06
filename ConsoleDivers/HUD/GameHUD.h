#pragma once

#include <memory>

class Player;
class WeaponSlotHUD;
class AmmoHUD;
class NumberSprite;
class PlayerStatusHUD;

namespace Craft
{
	class HUDCanvas;
}

class GameHUD
{
public:
	GameHUD();
	~GameHUD();

public:
	void Initialize(const std::shared_ptr<Player>& player);

	void Update();

private:
	void UpdateLayout();

private:
	std::shared_ptr<Craft::HUDCanvas> canvas;

	// HUD에서 사용하는 실제 숫자 Sprite 리소스
	std::unique_ptr<NumberSprite> numberSpriteSet;

	std::unique_ptr<WeaponSlotHUD> weaponSlotHUD;

	std::unique_ptr<AmmoHUD> ammoHUD;

	std::unique_ptr<PlayerStatusHUD> playerStatusHUD;
};