#include "GameHUD.h"
#include <Render/Renderer.h>
#include <Camera/Camera.h>
#include <HUD/HUDLayoutContext.h>
#include <HUD/Canvas/HUDCanvas.h>
#include <HUD/Manager/HUDManager.h>
#include <HUD/Equipment/WeaponSlotHUD.h>
#include <HUD/Equipment/AmmoHUD.h>
#include <HUD/Resource/Number/NumberSprite.h>
#include <HUD/Player/PlayerStatusHUD.h>
#include <HUD/Stratagem/StratagemHUD.h>

GameHUD::GameHUD() = default;

GameHUD::~GameHUD()
{
	// HUDManager가 shared_ptr로 Canvas를 보관하므로 GameHUD가 사라질 때 반드시 등록 해제
	// 그렇지 않으면 레벨 전환 후에도 이전 HUD가 남음
	// ------------------------------------------------------------
	if (canvas)
	{
		Craft::HUDManager::Get().RemoveCanvas(canvas);
	}
}

void GameHUD::Initialize(const std::shared_ptr<Player>& player)
{
	if (!player)
	{
		return;
	}

	canvas = std::make_shared<Craft::HUDCanvas>();

	// HUD 공용 숫자 Sprite
	numberSpriteSet = std::make_unique<NumberSprite>();

	// Weapon Slot
	weaponSlotHUD = std::make_unique<WeaponSlotHUD>();
	weaponSlotHUD->Initialize(canvas, player);

	// Ammo
	ammoHUD = std::make_unique<AmmoHUD>();
	ammoHUD->Initialize(canvas, player, numberSpriteSet->GetDigitPointers(), numberSpriteSet->GetInfinitySprite());

	// Player Status
	playerStatusHUD = std::make_unique<PlayerStatusHUD>();
	playerStatusHUD->Initialize(canvas, player);

	// Stratagem HUD
	stratagemHUD =std::make_unique<StratagemHUD>();
	stratagemHUD->Initialize(canvas, player);

	// HUD 최종 위치 계산
	UpdateLayout();

	// 엔진 HUDManager에 Game Canvas 등록
	Craft::HUDManager::Get().AddCanvas(canvas);
}

void GameHUD::Update()
{
	if (weaponSlotHUD)
	{
		weaponSlotHUD->Update();
	}

	if (ammoHUD)
	{
		ammoHUD->Update();
	}

	if (playerStatusHUD) 
	{
		playerStatusHUD->Update();
	}

	if (stratagemHUD)
	{
		stratagemHUD->Update();
	}
}

void GameHUD::UpdateLayout()
{
	Craft::Renderer& renderer = Craft::Renderer::Get();

	HUDLayoutContext context;

	// HUD 포함 전체 출력 영역
	context.screenSize = renderer.GetScreenSize();

	// 게임 월드 전용 영역
	context.viewportSize = renderer.GetCamera().GetViewportSize();

	if (weaponSlotHUD)
	{
		weaponSlotHUD->UpdateLayout(context);
	}

	if (ammoHUD)
	{
		ammoHUD->UpdateLayout(context);
	}

	if (playerStatusHUD)
	{
		playerStatusHUD->UpdateLayout(context);
	}

	if (stratagemHUD)
	{
		stratagemHUD->UpdateLayout(context);
	}
}
