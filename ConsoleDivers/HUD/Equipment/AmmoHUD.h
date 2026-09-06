#pragma once

#include <HUD/HUDLayoutContext.h>

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

	void UpdateLayout(const HUDLayoutContext& context);

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

	// HUD
	static constexpr int LeftMargin = 4;

	static constexpr int SlotWidth = 22;
	static constexpr int SlotHeight = 7;

	static constexpr int SlotSpacing = 2;

	static constexpr int BottomAreaTopMargin = 2;

	static constexpr int ContentOffsetY = 1;

	static constexpr int CurrentAmmoOffsetX = 3;
	static constexpr int ReserveMagazineOffsetX = 13;
	static constexpr int InfiniteAmmoOffsetX = 8;

	// Primary + Gap + Support + Gap 뒤
	static constexpr int AmmoOffsetX = LeftMargin + SlotWidth + SlotSpacing + SlotWidth + SlotSpacing;
};