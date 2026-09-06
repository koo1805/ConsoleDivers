#include "PlayerStatusHUD.h"

#include <HUD/Canvas/HUDCanvas.h>
#include <HUD/Panel/HUDPanel.h>
#include <HUD/Border/HUDBorder.h>
#include <HUD/ProgressBar/HUDProgressBar.h>
#include <Math/ColorRGB.h>
#include <Actor/Player/Player.h>

void PlayerStatusHUD::Initialize(const std::shared_ptr<Craft::HUDCanvas>& canvas, const std::shared_ptr<Player>& player)
{
	if (!canvas || !player)
	{
		return;
	}

	this->player = player;

	const Craft::Vector2 initialPosition = Craft::Vector2::Zero;

	const Craft::Vector2 panelSize(PanelWidth, PanelHeight);

	const Craft::ColorRGB panelColor(20, 20, 20);

	const Craft::ColorRGB borderColor(90, 90, 90);

	// Panel
	// ============================================================
	panel = std::make_shared<Craft::HUDPanel>(initialPosition, panelSize, panelColor);

	panel->SetSortingOrder(1000);

	canvas->AddWidget(panel);

	// Border
	// ============================================================
	border = std::make_shared<Craft::HUDBorder>(initialPosition, panelSize, borderColor);

	border->SetSortingOrder(1010);

	canvas->AddWidget(border);

	// HP Bar 빨강 계열
	// ============================================================
	const Craft::Vector2 healthPosition = Craft::Vector2::Zero;

	const Craft::Vector2 barSize(BarWidth, BarHeight);

	const Craft::ColorRGB healthColor(190, 45, 45);

	const Craft::ColorRGB emptyColor(45, 45, 45);

	healthBar = std::make_shared<Craft::HUDProgressBar>(healthPosition, barSize, static_cast<float>(player->GetMaxHealth()), healthColor, emptyColor);

	healthBar->SetSortingOrder(1020);

	canvas->AddWidget(healthBar);

	// Stamina Bar HP 아래쪽에 배치
	// ============================================================
	const Craft::Vector2 staminaPosition = Craft::Vector2::Zero;

	const Craft::ColorRGB staminaColor(215, 195, 55);

	staminaBar = std::make_shared<Craft::HUDProgressBar>(staminaPosition, barSize, player->GetMaxStamina(), staminaColor, emptyColor);

	staminaBar->SetSortingOrder(1020);

	canvas->AddWidget(staminaBar);

	// 생성 직후 현재 상태 반영
	Update();
}

void PlayerStatusHUD::Update()
{
	std::shared_ptr<Player> ownerPlayer = player.lock();

	if (!ownerPlayer)
	{
		panel->SetVisible(false);
		border->SetVisible(false);
		healthBar->SetVisible(false);
		staminaBar->SetVisible(false);

		return;
	}

	// Player가 존재하면 HUD 표시
	panel->SetVisible(true);
	border->SetVisible(true);
	healthBar->SetVisible(true);
	staminaBar->SetVisible(true);

	// HP
	// ============================================================
	healthBar->SetMaxValue(static_cast<float>(ownerPlayer->GetMaxHealth()));

	healthBar->SetValue(static_cast<float>(ownerPlayer->GetCurrentHealth()));

	// Stamina
	// ============================================================
	staminaBar->SetMaxValue(ownerPlayer->GetMaxStamina());

	staminaBar->SetValue(ownerPlayer->GetCurrentStamina());
}

void PlayerStatusHUD::UpdateLayout(const HUDLayoutContext& context)
{
	const Craft::Vector2 panelPosition(context.viewportSize.x + RightAreaLeftMargin, context.viewportSize.y - PanelHeight - BottomMargin);

	// 전체 Screen 오른쪽을 넘어가는 경우 방어
	if (panelPosition.x + PanelWidth > context.screenSize.x)
	{
		return;
	}

	if (panelPosition.y < 0)
	{
		return;
	}

	panel->SetPosition(panelPosition);
	border->SetPosition(panelPosition);

	healthBar->SetPosition(Craft::Vector2(panelPosition.x + BarOffsetX, panelPosition.y + HealthOffsetY));
	staminaBar->SetPosition(Craft::Vector2(panelPosition.x + BarOffsetX, panelPosition.y + StaminaOffsetY));
}