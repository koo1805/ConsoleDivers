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

	// 임시 위치
	// 최종 Layout 정리 전까지 ScreenSpace 절대좌표 사용
	// 이후 viewport 기준으로 Anchor 처리 예정
	// ============================================================

	const Craft::Vector2 panelPosition(90, 30);

	const Craft::Vector2 panelSize(26, 9);

	const Craft::ColorRGB panelColor(20, 20, 20);

	const Craft::ColorRGB borderColor(90, 90, 90);

	// Panel
	// ============================================================
	panel = std::make_shared<Craft::HUDPanel>(panelPosition, panelSize, panelColor);

	panel->SetSortingOrder(1000);

	canvas->AddWidget(panel);

	// Border
	// ============================================================
	border = std::make_shared<Craft::HUDBorder>(panelPosition, panelSize, borderColor);

	border->SetSortingOrder(1010);

	canvas->AddWidget(border);

	// HP Bar 빨강 계열
	// ============================================================
	const Craft::Vector2 healthPosition(panelPosition.x + 2, panelPosition.y + 2);

	const Craft::Vector2 barSize(22, 2);

	const Craft::ColorRGB healthColor(190, 45, 45);

	const Craft::ColorRGB emptyColor(45, 45, 45);

	healthBar = std::make_shared<Craft::HUDProgressBar>(healthPosition, barSize, static_cast<float>(player->GetMaxHealth()), healthColor, emptyColor);

	healthBar->SetSortingOrder(1020);

	canvas->AddWidget(healthBar);

	// Stamina Bar HP 아래쪽에 배치
	// ============================================================
	const Craft::Vector2 staminaPosition(panelPosition.x + 2, panelPosition.y + 5);

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