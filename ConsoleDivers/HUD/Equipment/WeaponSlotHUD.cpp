#include "WeaponSlotHUD.h"
#include <Actor/Player/Player.h>
#include <Actor/Weapon/WeaponBase.h>

#include <HUD/Canvas/HUDCanvas.h>
#include <HUD/Panel/HUDPanel.h>
#include <HUD/Border/HUDBorder.h>
#include <HUD/Sprite/HUDSprite.h>
#include <HUD/HUDLayoutContext.h>

#include <Math/ColorRGB.h>

void WeaponSlotHUD::Initialize(const std::shared_ptr<Craft::HUDCanvas>& canvas, const std::shared_ptr<Player>& player)
{
	if (!canvas || !player)
	{
		return;
	}

	this->player = player;

	const Craft::Vector2 initialPosition = Craft::Vector2::Zero;
	const Craft::Vector2 slotSize(SlotWidth, SlotHeight);

	// HUD 색상
	// 아직 실제 최종 디자인 단계가 아니므로 색상은 여기서 임시로 지정

	const Craft::ColorRGB panelColor(20, 20, 20);

	const Craft::ColorRGB normalBorderColor(90, 90, 90);

	const Craft::ColorRGB selectedBorderColor(235, 205, 45);

	// Primary Slot
	// ============================================================
	primaryPanel =std::make_shared<Craft::HUDPanel>(initialPosition, slotSize, panelColor);

	primaryPanel->SetSortingOrder(1000);

	canvas->AddWidget(primaryPanel);

	primaryBorder = std::make_shared<Craft::HUDBorder>(initialPosition, slotSize, selectedBorderColor);

	primaryBorder->SetSortingOrder(1010);

	canvas->AddWidget(primaryBorder);

	// Weapon Sprite는 슬롯 안쪽으로 약간 띄워 배치
	// ------------------------------------------------------------
	primaryWeaponSprite = std::make_shared<Craft::HUDSprite>(nullptr, Craft::Vector2(initialPosition.x + WeaponSpriteOffsetX, initialPosition.y + WeaponSpriteOffsetY));

	primaryWeaponSprite->SetSortingOrder(1020);

	canvas->AddWidget(primaryWeaponSprite);

	// Support Slot -> Primary 오른쪽에 배치
	// ============================================================
	const Craft::Vector2 supportPosition(initialPosition.x + SlotWidth + SlotSpacing, initialPosition.y);

	supportPanel = std::make_shared<Craft::HUDPanel>(supportPosition, slotSize, panelColor);

	supportPanel->SetSortingOrder(1000);

	canvas->AddWidget(supportPanel);

	supportBorder = std::make_shared<Craft::HUDBorder>(supportPosition, slotSize, normalBorderColor);

	supportBorder->SetSortingOrder(1010);

	canvas->AddWidget(supportBorder);

	supportWeaponSprite = std::make_shared<Craft::HUDSprite>(nullptr, Craft::Vector2(supportPosition.x + WeaponSpriteOffsetX, supportPosition.y + WeaponSpriteOffsetY));

	supportWeaponSprite->SetSortingOrder(1020);

	canvas->AddWidget(supportWeaponSprite);

	// 처음 생성될 때 Player 상태를 즉시 HUD에 반영
	Update();
}

void WeaponSlotHUD::Update()
{
	std::shared_ptr<Player> ownerPlayer = player.lock();

	if (!ownerPlayer)
	{
		return;
	}

	// Primary Weapon
	// ============================================================
	std::shared_ptr<WeaponBase> primaryWeapon = ownerPlayer->GetPrimaryWeapon();

	if (primaryWeapon)
	{
		primaryPanel->SetVisible(true);
		primaryBorder->SetVisible(true);

		primaryWeaponSprite->SetSprite(primaryWeapon->GetHUDSprite());

		primaryWeaponSprite->SetVisible(true);
	}
	else
	{
		// 원칙상 Primary는 항상 존재하지만 방어적으로 처리
		primaryPanel->SetVisible(false);
		primaryBorder->SetVisible(false);
		primaryWeaponSprite->SetVisible(false);
	}

	// Support Weapon
	// 지원무기가 있을 때만 2번 슬롯 표시
	// ============================================================
	std::shared_ptr<WeaponBase> supportWeapon = ownerPlayer->GetSupportWeapon();

	const bool hasSupportWeapon = supportWeapon != nullptr;

	supportPanel->SetVisible(hasSupportWeapon);

	supportBorder->SetVisible(hasSupportWeapon);

	supportWeaponSprite->SetVisible(hasSupportWeapon);

	if (supportWeapon)
	{
		supportWeaponSprite->SetSprite(supportWeapon->GetHUDSprite());
	}

	// 현재 선택 슬롯
	// 선택 슬롯의 Border만 활성 상태의 색으로 표시
	// 현재 HUDBorder에는 색 변경 함수가 이미 있으므로 Sprite 자체를 다시 만들 필요 없음
	// ============================================================
	const Craft::ColorRGB normalBorderColor(90, 90, 90);

	const Craft::ColorRGB selectedBorderColor(235, 205, 45);

	const WeaponSlotType activeSlot = ownerPlayer->GetActiveWeaponSlot();

	primaryBorder->SetBorderColor(activeSlot == WeaponSlotType::Primary ? selectedBorderColor : normalBorderColor);

	if (hasSupportWeapon)
	{
		supportBorder->SetBorderColor(activeSlot == WeaponSlotType::Support ? selectedBorderColor : normalBorderColor);
	}
}

void WeaponSlotHUD::UpdateLayout(const HUDLayoutContext& context)
{
	// Viewport 아래쪽이 Bottom HUD 영역의 시작선
	// ------------------------------------------------------------
	const Craft::Vector2 primaryPosition(LeftMargin, context.viewportSize.y + BottomAreaTopMargin);

	// ScreenBuffer 하단을 넘어가는 잘못된 Setting 방어
	if (primaryPosition.y + SlotHeight > context.screenSize.y)
	{
		return;
	}

	primaryPanel->SetPosition(primaryPosition);
	primaryBorder->SetPosition(primaryPosition);

	primaryWeaponSprite->SetPosition(Craft::Vector2(primaryPosition.x + WeaponSpriteOffsetX, primaryPosition.y + WeaponSpriteOffsetY));

	// Support Slot은 Primary 오른쪽
	const Craft::Vector2 supportPosition(primaryPosition.x + SlotWidth + SlotSpacing, primaryPosition.y);

	supportPanel->SetPosition(supportPosition);
	supportBorder->SetPosition(supportPosition);
	supportWeaponSprite->SetPosition(Craft::Vector2(supportPosition.x + WeaponSpriteOffsetX, supportPosition.y +WeaponSpriteOffsetY));
}
