#pragma once

#include <Level/Level.h>

#include <memory>

namespace Craft
{
	class HUDCanvas;
	class HUDSprite;
	class PixelSprite;
}

class MainMenuLevel : public Craft::Level
{
	TYPE_DECLARATIONS(MainMenuLevel, Level)

public:
	MainMenuLevel();
	virtual ~MainMenuLevel() override;

public:
	// 메뉴 HUD 생성
	virtual void OnInitialized() override;

	// Hover / Click 처리
	virtual void Tick(float deltaTime) override;

	// 메뉴 HUD 출력
	virtual void Draw() override;

private:
	// 제목 / 버튼 Sprite 생성
	void CreateMenuSprites();

	// HUD 위치 계산 및 Canvas 등록
	void CreateMenuHUD();

	// 마우스가 HUD 영역 안에 있는지 확인
	bool IsMouseOver(const std::shared_ptr<Craft::HUDSprite>& widget) const;

private:
	// 메뉴 전체를 관리할 Canvas
	std::shared_ptr<Craft::HUDCanvas> canvas;

	// --------------------------------------------------------
	// 실제 Sprite 리소스
	// HUDSprite는 PixelSprite를 참조만 하기 때문에
	// MainMenuLevel이 Sprite의 생명주기를 관리한다.
	// --------------------------------------------------------

	std::unique_ptr<Craft::PixelSprite> titleSprite;

	std::unique_ptr<Craft::PixelSprite> startButtonSprite;

	std::unique_ptr<Craft::PixelSprite> quitButtonSprite;

	// --------------------------------------------------------
	// 화면에 출력되는 HUD Widget
	// --------------------------------------------------------

	std::shared_ptr<Craft::HUDSprite> titleHUD;

	std::shared_ptr<Craft::HUDSprite> startButtonHUD;

	std::shared_ptr<Craft::HUDSprite> quitButtonHUD;

	// 현재 Hover 상태
	bool startHovered = false;
	bool quitHovered = false;
};