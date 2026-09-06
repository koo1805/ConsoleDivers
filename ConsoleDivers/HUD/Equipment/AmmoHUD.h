#pragma once

#include <array>
#include <memory>

class Player;

namespace Craft
{
	class HUDCanvas;
	class HUDPanel;
	class HUDBorder;
	class HUDNumber;
	class HUDSprite;
	class PixelSprite;
}

class AmmoHUD
{
public:
	AmmoHUD() = default;
	~AmmoHUD() = default;

public:
	void Initialize(
		const std::shared_ptr<Craft::HUDCanvas>& canvas,
		const std::shared_ptr<Player>& player,
		const std::array<const Craft::PixelSprite*, 10>& digitSprites,
		const Craft::PixelSprite* infinitySprite);

	void Update();

private:
	// HUD는 Player를 소유하지 않음
	std::weak_ptr<Player> player;

	std::shared_ptr<Craft::HUDPanel> panel;

	std::shared_ptr<Craft::HUDBorder> border;

	// 현재 탄창 안의 남은 탄
	std::shared_ptr<Craft::HUDNumber> currentAmmoNumber;

	// 남아있는 예비 탄창 수
	std::shared_ptr<Craft::HUDNumber> reserveMagazineNumber;

	// 무한 Sprite
	std::shared_ptr<Craft::HUDSprite> infiniteAmmoSprite;
};