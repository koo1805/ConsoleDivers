#pragma once

#include <HUD/HUDLayoutContext.h>

#include <memory>

class Player;

namespace Craft
{
	class HUDCanvas;
	class HUDPanel;
	class HUDBorder;
	class HUDProgressBar;
}

class PlayerStatusHUD
{
public:
	PlayerStatusHUD() = default;
	~PlayerStatusHUD() = default;

public:
	// HUD Widget 생성 및 Canvas 등록
	void Initialize(const std::shared_ptr<Craft::HUDCanvas>& canvas, const std::shared_ptr<Player>& player);

	// Player 상태값을 HUD에 반영
	void Update();

	void UpdateLayout(const HUDLayoutContext& context);

private:
	// ------------------------------------------------------------
	// HUD는 Player 생명주기를 소유하지 않는다.
	// ------------------------------------------------------------
	std::weak_ptr<Player> player;

	// 전체 상태 영역 배경
	std::shared_ptr<Craft::HUDPanel> panel;

	// 전체 상태 영역 테두리
	std::shared_ptr<Craft::HUDBorder> border;

	// HP
	std::shared_ptr<Craft::HUDProgressBar> healthBar;

	// Stamina
	std::shared_ptr<Craft::HUDProgressBar> staminaBar;

	// HUD
	static constexpr int RightAreaLeftMargin = 2;
	static constexpr int BottomMargin = 2;

	static constexpr int PanelWidth = 26;
	static constexpr int PanelHeight = 9;

	static constexpr int BarOffsetX = 2;

	static constexpr int HealthOffsetY = 2;
	static constexpr int StaminaOffsetY = 5;

	static constexpr int BarWidth = 22;
	static constexpr int BarHeight = 2;
};