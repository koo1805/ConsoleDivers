#include "AmmoHUD.h"

#include <Actor/Player/Player.h>
#include <Actor/Weapon/WeaponBase.h>

#include <HUD/Canvas/HUDCanvas.h>
#include <HUD/Panel/HUDPanel.h>
#include <HUD/Border/HUDBorder.h>
#include <HUD/Number/HUDNumber.h>
#include <HUD/Sprite/HUDSprite.h>

#include <Math/ColorRGB.h>

void AmmoHUD::Initialize(
	const std::shared_ptr<Craft::HUDCanvas>& canvas,
	const std::shared_ptr<Player>& player,
	const std::array<const Craft::PixelSprite*, 10>& digitSprites,
	const Craft::PixelSprite* infinitySprite)
{
	if (!canvas || !player)
	{
		return;
	}

	this->player = player;

	// Slot 1 = x 4
	// Slot 2 = x 28
	// Ammo는 항상 세 번째 슬롯 위치에 고정
	// Support Weapon이 없어도 위치는 변하지 않음
	// ------------------------------------------------------------
	const Craft::Vector2 initialPosition = Craft::Vector2::Zero;

	const Craft::Vector2 ammoSize(SlotWidth, SlotHeight);

	const Craft::ColorRGB panelColor(20, 20, 20);

	const Craft::ColorRGB borderColor(90, 90, 90);

	panel = std::make_shared<Craft::HUDPanel>(initialPosition, ammoSize, panelColor);

	panel->SetSortingOrder(1000);

	canvas->AddWidget(panel);

	border = std::make_shared<Craft::HUDBorder>(initialPosition, ammoSize, borderColor);

	border->SetSortingOrder(1010);

	canvas->AddWidget(border);

	// 현재 탄약
	// ============================================================
	currentAmmoNumber = std::make_shared<Craft::HUDNumber>(Craft::Vector2::Zero);

	currentAmmoNumber->SetDigitSprites(digitSprites);

	// 6 -> 06
	currentAmmoNumber->SetMinDigits(2);

	currentAmmoNumber->SetDigitSpacing(1);

	currentAmmoNumber->SetSortingOrder(1020);

	canvas->AddWidget(currentAmmoNumber);

	// 예비 탄창 수
	// 현재 WeaponData가 reserveMagazineCount를 사용하므로 남은 총알 수가 아니라 "남은 탄창 개수"를 표시한다.
	// ============================================================
	reserveMagazineNumber = std::make_shared<Craft::HUDNumber>(Craft::Vector2(Craft::Vector2::Zero));

	reserveMagazineNumber->SetDigitSprites(digitSprites);

	reserveMagazineNumber->SetMinDigits(2);

	reserveMagazineNumber->SetDigitSpacing(1);

	reserveMagazineNumber->SetSortingOrder(1020);

	canvas->AddWidget(reserveMagazineNumber);

	// 무한 Sprite
	infiniteAmmoSprite = std::make_shared<Craft::HUDSprite>(infinitySprite, Craft::Vector2::Zero);

	infiniteAmmoSprite->SetSortingOrder(1020);

	// 기본은 숨김
	infiniteAmmoSprite->SetVisible(false);

	canvas->AddWidget(infiniteAmmoSprite);

	Update();
}

void AmmoHUD::Update()
{
	std::shared_ptr<Player> ownerPlayer = player.lock();

	if (!ownerPlayer)
	{
		return;
	}

	// 현재 Player가 실제 손에 들고 있는 무기
	// Primary/Support 슬롯을 HUD가 직접 판단하지 않음
	// Player가 정한 결과만 읽음
	// ------------------------------------------------------------
	std::shared_ptr<WeaponBase> equippedWeapon = ownerPlayer->GetEquippedWeapon();

	if (!equippedWeapon)
	{
		panel->SetVisible(false);
		border->SetVisible(false);
		currentAmmoNumber->SetVisible(false);
		reserveMagazineNumber->SetVisible(false);
		infiniteAmmoSprite->SetVisible(false);

		return;
	}

	panel->SetVisible(true);
	border->SetVisible(true);

	// 무한 탄약
	// ------------------------------------------------------------
	if (equippedWeapon->IsInfiniteAmmo())
	{
		currentAmmoNumber->SetVisible(false);
		reserveMagazineNumber->SetVisible(false);

		infiniteAmmoSprite->SetVisible(true);

		return;
	}

	// 일반 탄약
	// ------------------------------------------------------------
	infiniteAmmoSprite->SetVisible(false);

	currentAmmoNumber->SetVisible(true);
	reserveMagazineNumber->SetVisible(true);

	// 현재 탄창
	currentAmmoNumber->SetValue(equippedWeapon->GetCurrentAmmo());

	// 남아있는 예비 탄창 수
	reserveMagazineNumber->SetValue(equippedWeapon->GetReserveMagazineCount());
}

void AmmoHUD::UpdateLayout(const HUDLayoutContext& context)
{
	const Craft::Vector2 ammoPosition(AmmoOffsetX, context.viewportSize.y + BottomAreaTopMargin);

	if (ammoPosition.y + SlotHeight > context.screenSize.y)
	{
		return;
	}

	panel->SetPosition(ammoPosition);
	border->SetPosition(ammoPosition);

	currentAmmoNumber->SetPosition(Craft::Vector2(ammoPosition.x + CurrentAmmoOffsetX, ammoPosition.y + ContentOffsetY));

	reserveMagazineNumber->SetPosition(Craft::Vector2(ammoPosition.x + ReserveMagazineOffsetX, ammoPosition.y + ContentOffsetY));

	infiniteAmmoSprite->SetPosition(Craft::Vector2(ammoPosition.x + InfiniteAmmoOffsetX, ammoPosition.y + ContentOffsetY));
}
