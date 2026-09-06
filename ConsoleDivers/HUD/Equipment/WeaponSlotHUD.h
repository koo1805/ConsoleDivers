#pragma once

#include <HUD/HUDLayoutContext.h>

#include <memory>

class Player;

namespace Craft
{
	class HUDCanvas;
	class HUDPanel;
	class HUDBorder;
	class HUDSprite;
}

class WeaponSlotHUD
{
public:
	WeaponSlotHUD() = default;
	~WeaponSlotHUD() = default;

public:
	// 무기 슬롯 HUD 생성
	// 외부 GameHUD가 가지고 있는 Canvas에 필요한 Widget들을 등록
	void Initialize(const std::shared_ptr<Craft::HUDCanvas>& canvas, const std::shared_ptr<Player>& player);

	// Player의 현재 무기 상태를 HUD에 반영
	void Update();

	void UpdateLayout(const HUDLayoutContext& context);

private:
	// HUD는 Player의 생명주기를 소유하면 안 되므로 weak_ptr 사용
	std::weak_ptr<Player> player;

	// Primary Weapon Slot
	// ------------------------------------------------------------
	std::shared_ptr<Craft::HUDPanel> primaryPanel;

	std::shared_ptr<Craft::HUDBorder> primaryBorder;

	std::shared_ptr<Craft::HUDSprite> primaryWeaponSprite;

	// Support Weapon Slot
	// ------------------------------------------------------------
	std::shared_ptr<Craft::HUDPanel> supportPanel;

	std::shared_ptr<Craft::HUDBorder> supportBorder;

	std::shared_ptr<Craft::HUDSprite> supportWeaponSprite;

	// // HUD 디자인 값
	static constexpr int LeftMargin = 4;
	static constexpr int BottomAreaTopMargin = 2;

	static constexpr int SlotWidth = 22;
	static constexpr int SlotHeight = 7;

	static constexpr int SlotSpacing = 2;

	static constexpr int WeaponSpriteOffsetX = 4;
	static constexpr int WeaponSpriteOffsetY = 2;
};