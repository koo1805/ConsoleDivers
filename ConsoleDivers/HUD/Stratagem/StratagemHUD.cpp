#include "StratagemHUD.h"

#include <Actor/Player/Player.h>

#include <Stratagem/StratagemSystem.h>
#include <Stratagem/Data/StratagemData.h>

#include <HUD/Canvas/HUDCanvas.h>
#include <HUD/Panel/HUDPanel.h>
#include <HUD/Border/HUDBorder.h>
#include <HUD/Sprite/HUDSprite.h>

#include <Math/ColorRGB.h>
#include <Render/Cell.h>

namespace
{
	// ========================================================
	// 방향 Sprite Cell 생성 보조 함수
	// ========================================================
	Craft::Cell MakeArrowPixel(const Craft::ColorRGB& color)
	{
		Craft::Cell cell;

		cell.character = ' ';
		cell.foreground = color;
		cell.background = color;

		return cell;
	}
}

void StratagemHUD::Initialize(const std::shared_ptr<Craft::HUDCanvas>& canvas, const std::shared_ptr<Player>& player)
{
	if (!canvas || !player)
	{
		return;
	}

	this->player = player;

	// 방향 Sprite 리소스 생성
	// 한 번만 만들어두고 HUDSprite들이 포인터로 참조
	upSprite = CreateArrowSprite(StratagemCommand::Up);

	downSprite = CreateArrowSprite(StratagemCommand::Down);

	leftSprite = CreateArrowSprite(StratagemCommand::Left);

	rightSprite = CreateArrowSprite(StratagemCommand::Right);

	const Craft::Vector2 initialPosition = Craft::Vector2::Zero;

	const Craft::Vector2 panelSize(PanelWidth, PanelHeight);

	const Craft::ColorRGB panelColor(20, 20, 20);

	const Craft::ColorRGB borderColor(90, 90, 90);

	// Panel
	// ========================================================
	panel = std::make_shared<Craft::HUDPanel>(initialPosition, panelSize, panelColor);

	panel->SetSortingOrder(1000);

	canvas->AddWidget(panel);

	// Border
	// ========================================================
	border = std::make_shared<Craft::HUDBorder>(initialPosition, panelSize, borderColor);

	border->SetSortingOrder(1010);

	canvas->AddWidget(border);

	// 현재 Stratagem의 Command 수만큼 Widget 생성
	// ========================================================
	const StratagemSystem& stratagemSystem = player->GetStratagemSystem();

	const StratagemData* stratagem = stratagemSystem.GetStratagemData(0);

	if (!stratagem)
	{
		return;
	}

	commandWidgets.clear();

	commandWidgets.reserve(stratagem->commandSequence.size());

	for (std::size_t index = 0; index < stratagem->commandSequence.size(); ++index)
	{
		const StratagemCommand command = stratagem->commandSequence[index];

		std::shared_ptr<Craft::HUDSprite> commandWidget = std::make_shared<Craft::HUDSprite>(GetCommandSprite(command), Craft::Vector2::Zero);

		commandWidget->SetSortingOrder(1020);

		canvas->AddWidget(commandWidget);

		commandWidgets.push_back(commandWidget);
	}

	// 최초 상태 반영
	Update();
}

Craft::PixelSprite
StratagemHUD::CreateArrowSprite(StratagemCommand command) const
{
	constexpr int width = CommandWidth;
	constexpr int height = CommandHeight;

	Craft::PixelSprite sprite(width, height);

	// Sprite 자체는 흰색으로 생성
	//
	// 입력 진행 상태에 따른
	// 회색 / 노랑 / 초록 / 빨강 색상은
	// HUDSprite의 Tint로 변경
	// ========================================================
	const Craft::ColorRGB whiteColor(255, 255, 255);

	const Craft::Cell pixel = MakeArrowPixel(whiteColor);

	// ========================================================
	// 5x5 방향 화살표
	// ========================================================
	switch (command)
	{
	case StratagemCommand::Up:
	{
		static const char* shape[height] =
		{
			"..X..",
			".XXX.",
			"XXXXX",
			"..X..",
			"..X.."
		};

		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				if (shape[y][x] == 'X')
				{
					sprite.SetCell(x, y, pixel);
				}
			}
		}

		break;
	}

	case StratagemCommand::Down:
	{
		static const char* shape[height] =
		{
			"..X..",
			"..X..",
			"XXXXX",
			".XXX.",
			"..X.."
		};

		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				if (shape[y][x] == 'X')
				{
					sprite.SetCell(x, y, pixel);
				}
			}
		}

		break;
	}

	case StratagemCommand::Left:
	{
		static const char* shape[height] =
		{
			"..X..",
			".XX..",
			"XXXXX",
			".XX..",
			"..X.."
		};

		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				if (shape[y][x] == 'X')
				{
					sprite.SetCell(x, y, pixel);
				}
			}
		}

		break;
	}

	case StratagemCommand::Right:
	{
		static const char* shape[height] =
		{
			"..X..",
			"..XX.",
			"XXXXX",
			"..XX.",
			"..X.."
		};

		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				if (shape[y][x] == 'X')
				{
					sprite.SetCell(x, y, pixel);
				}
			}
		}

		break;
	}

	default:
		break;
	}

	return sprite;
}

const Craft::PixelSprite* StratagemHUD::GetCommandSprite(StratagemCommand command) const
{
	switch (command)
	{
	case StratagemCommand::Up:
		return &upSprite;

	case StratagemCommand::Down:
		return &downSprite;

	case StratagemCommand::Left:
		return &leftSprite;

	case StratagemCommand::Right:
		return &rightSprite;

	default:
		return nullptr;
	}
}

void StratagemHUD::Update()
{
	std::shared_ptr<Player> ownerPlayer = player.lock();

	if (!ownerPlayer)
	{
		if (panel)
		{
			panel->SetVisible(false);
		}

		if (border)
		{
			border->SetVisible(false);
		}

		for (const auto& widget : commandWidgets)
		{
			if (widget)
			{
				widget->SetVisible(false);
			}
		}

		return;
	}

	const StratagemSystem& stratagemSystem = ownerPlayer->GetStratagemSystem();

	const StratagemState state = stratagemSystem.GetState();

	const std::size_t currentInputIndex = stratagemSystem.GetCurrentInputIndex();

	const StratagemInputResult inputResult = stratagemSystem.GetLastInputResult();

	panel->SetVisible(true);
	border->SetVisible(true);

	// 상태별 Border 색
	// ========================================================
	const Craft::ColorRGB idleBorderColor(90, 90, 90);

	const Craft::ColorRGB inputBorderColor(235, 205, 45);

	const Craft::ColorRGB readyBorderColor(65, 210, 90);

	const Craft::ColorRGB failedBorderColor(220, 55, 55);

	const Craft::ColorRGB cooldownBorderColor(80, 140, 220);

	if (inputResult == StratagemInputResult::Failed)
	{
		border->SetBorderColor(failedBorderColor);
	}
	else if (inputResult == StratagemInputResult::Cooldown)
	{
		border->SetBorderColor(cooldownBorderColor);
	}
	else
	{
		switch (state)
		{
		case StratagemState::Idle:
			border->SetBorderColor(idleBorderColor);
			break;

		case StratagemState::Inputting:
			border->SetBorderColor(inputBorderColor);
			break;

		case StratagemState::ReadyToThrow:
			border->SetBorderColor(readyBorderColor);
			break;

		default:
			break;
		}
	}

	// 커맨드별 상태 표시
	// ========================================================
	const Craft::ColorRGB idleColor(105, 105, 105);

	const Craft::ColorRGB completedColor(65, 210, 90);

	const Craft::ColorRGB currentColor(235, 205, 45);

	const Craft::ColorRGB readyColor(70, 230, 100);

	const Craft::ColorRGB failedColor(220, 55, 55);

	const Craft::ColorRGB cooldownColor(80, 140, 220);

	for (std::size_t index = 0; index < commandWidgets.size(); ++index)
	{
		const std::shared_ptr<Craft::HUDSprite>& widget = commandWidgets[index];

		if (!widget)
		{
			continue;
		}

		widget->SetVisible(true);

		// 잘못된 방향 입력 후
		// ====================================================
		if (inputResult == StratagemInputResult::Failed)
		{
			widget->SetTintColor(failedColor);

			continue;
		}

		// 쿨타임
		// ====================================================
		if (inputResult == StratagemInputResult::Cooldown)
		{
			widget->SetTintColor(cooldownColor);

			continue;
		}

		// 입력 완료 상태
		// 모든 커맨드를 녹색으로 표시
		// ====================================================
		if (state == StratagemState::ReadyToThrow)
		{
			widget->SetTintColor(readyColor);

			continue;
		}

		// 아직 스트라타젬 입력 중이 아님
		// ====================================================
		if (state == StratagemState::Idle)
		{
			widget->SetTintColor(idleColor);

			continue;
		}

		// 이미 성공한 입력
		// ====================================================
		if (index < currentInputIndex)
		{
			widget->SetTintColor(completedColor);

			continue;
		}

		// 지금 입력해야 할 방향
		// ====================================================
		if (index == currentInputIndex)
		{
			widget->SetTintColor(currentColor);

			continue;
		}

		// 아직 입력하지 않은 뒤쪽 커맨드
		widget->SetTintColor(idleColor);
	}
}

void StratagemHUD::UpdateLayout(const HUDLayoutContext& context)
{
	// Viewport 오른쪽이 HUD 영역의 시작선
	// ========================================================
	const Craft::Vector2 panelPosition(context.viewportSize.x + RightAreaLeftMargin, TopMargin);

	// 전체 ScreenBuffer를 넘어가는 설정 방어
	// ========================================================
	if (panelPosition.x + PanelWidth > context.screenSize.x)
	{
		return;
	}

	if (panelPosition.y + PanelHeight > context.screenSize.y)
	{
		return;
	}

	panel->SetPosition(panelPosition);

	border->SetPosition(panelPosition);

	// Command Widget 위치
	// ========================================================
	for (std::size_t index = 0; index < commandWidgets.size(); ++index)
	{
		const std::shared_ptr<Craft::HUDSprite>& widget = commandWidgets[index];

		if (!widget)
		{
			continue;
		}

		const int commandX = panelPosition.x + CommandOffsetX + static_cast<int>(index) * (CommandWidth + CommandSpacing);

		const int commandY = panelPosition.y + CommandOffsetY;

		widget->SetPosition(Craft::Vector2(commandX, commandY));
	}
}
