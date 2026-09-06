#pragma once

#include <HUD/HUDLayoutContext.h>
#include <Render/Sprite/PixelSprite.h>

#include <memory>
#include <vector>

class Player;

enum class StratagemCommand;

namespace Craft
{
	class HUDCanvas;
	class HUDPanel;
	class HUDBorder;
	class HUDSprite;
}

class StratagemHUD
{
public:
	StratagemHUD() = default;
	~StratagemHUD() = default;

public:
	// Stratagem HUD Widget 생성
	// HUD는 Player를 소유 X
	// Player의 상태를 읽기만 하기 때문에 weak_ptr 사용
	void Initialize(const std::shared_ptr<Craft::HUDCanvas>& canvas, const std::shared_ptr<Player>& player);

	// 현재 StratagemSystem 상태를 HUD에 반영
	void Update();

	// 화면 / Viewport 설정값 기준 위치 계산
	void UpdateLayout(const HUDLayoutContext& context);

private:
	// 방향 커맨드에 맞는 Sprite 반환
	const Craft::PixelSprite* GetCommandSprite(StratagemCommand command) const;

	// 방향 Sprite 생성
	Craft::PixelSprite CreateArrowSprite(StratagemCommand command) const;

private:
	// HUD는 Player의 생명주기를 관리하지 않음
	std::weak_ptr<Player> player;

	// 전체 영역
	std::shared_ptr<Craft::HUDPanel> panel;

	std::shared_ptr<Craft::HUDBorder> border;

	// 방향 커맨드 HUD
	std::vector<std::shared_ptr<Craft::HUDSprite>> commandWidgets;

	// 방향 Sprite 리소스
	Craft::PixelSprite upSprite;
	Craft::PixelSprite downSprite;
	Craft::PixelSprite leftSprite;
	Craft::PixelSprite rightSprite;

private:
	// HUD Layout 값

	// Viewport 오른쪽으로부터의 간격
	static constexpr int RightAreaLeftMargin = 2;

	// 화면 상단 여백
	static constexpr int TopMargin = 2;

	// Panel 크기
	static constexpr int PanelWidth = 36;
	static constexpr int PanelHeight = 9;

	// 커맨드 시작 위치
	static constexpr int CommandOffsetX = 3;
	static constexpr int CommandOffsetY = 2;

	// 방향 Sprite 크기
	static constexpr int CommandWidth = 5;
	static constexpr int CommandHeight = 5;

	// 커맨드 사이 간격
	static constexpr int CommandSpacing = 1;
};