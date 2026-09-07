#include "WeaponGaugeHUD.h"

#include <Actor/Player/Player.h>
#include <Actor/Weapon/WeaponBase.h>

#include <HUD/Canvas/HUDCanvas.h>
#include <HUD/ProgressBar/HUDProgressBar.h>

#include <Render/Renderer.h>
#include <Camera/Camera.h>

#include <Math/ColorRGB.h>

void WeaponGaugeHUD::Initialize(const std::shared_ptr<Craft::HUDCanvas>& canvas, const std::shared_ptr<Player>& player)
{
	if (!canvas || !player)
	{
		return;
	}

	this->player = player;

	const Craft::Vector2 initialPosition = Craft::Vector2::Zero;

	const Craft::Vector2 barSize(BarWidth, BarHeight);

	// 기본값은 Reload 색상
	const Craft::ColorRGB fillColor(220, 190, 60);

	const Craft::ColorRGB emptyColor(45, 45, 45);

	actionBar = std::make_shared<Craft::HUDProgressBar>(
			initialPosition,
			barSize,
			1.0f,
			fillColor,
			emptyColor
		);

	actionBar->SetSortingOrder(1050);

	// 처음에는 숨김
	actionBar->SetVisible(false);

	canvas->AddWidget(actionBar);
}

void WeaponGaugeHUD::Update()
{
	if (!actionBar)
	{
		return;
	}

	std::shared_ptr<Player> ownerPlayer = player.lock();

	if (!ownerPlayer)
	{
		actionBar->SetVisible(false);

		return;
	}

	std::shared_ptr<WeaponBase> weapon = ownerPlayer->GetEquippedWeapon();

	if (!weapon)
	{
		actionBar->SetVisible(false);

		return;
	}

	// Reload
	// ============================================================
	if (weapon->IsReloading())
	{
		const Craft::ColorRGB reloadColor(220, 190, 60);

		actionBar->SetFillColor(reloadColor);

		actionBar->SetMaxValue(1.0f);

		actionBar->SetValue(weapon->GetReloadProgress());

		actionBar->SetVisible(true);
	}

	// ============================================================
	// Charge
	// ============================================================
	else if (weapon->IsCharging())
	{
		const Craft::ColorRGB chargeColor(70, 170, 255);

		actionBar->SetFillColor(chargeColor);

		actionBar->SetMaxValue(1.0f);

		actionBar->SetValue(weapon->GetChargeProgress());

		actionBar->SetVisible(true);
	}

	// Reload / Charge 둘 다 아니면 숨김
	else
	{
		actionBar->SetVisible(false);

		return;
	}

	// ============================================================
	// Player World Position -> Screen Position
	// ============================================================
	Craft::Renderer& renderer = Craft::Renderer::Get();

	const Craft::Vector2 playerScreenPosition = renderer.GetCamera().WorldToScreen(ownerPlayer->GetPosition());

	// Player 바로 아래
	const Craft::Vector2 barPosition(playerScreenPosition.x, playerScreenPosition.y + PlayerBottomOffset);

	actionBar->SetPosition(barPosition);
}