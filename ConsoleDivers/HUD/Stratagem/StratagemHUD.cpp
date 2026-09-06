#include "StratagemHUD.h"
#include <Actor/Player/Player.h>
#include <Render/Cell.h>
#include <Math/ColorRGB.h>
#include <Stratagem/StratagemSystem.h>
#include <Stratagem/Data/StratagemData.h>
#include <HUD/Canvas/HUDCanvas.h>
#include <HUD/Panel/HUDPanel.h>
#include <HUD/Border/HUDBorder.h>
#include <HUD/Sprite/HUDSprite.h>
#include <HUD/Number/HUDNumber.h>

#include <algorithm>
#include <cmath>

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

void StratagemHUD::Initialize(
	const std::shared_ptr<Craft::HUDCanvas>& canvas,
	const std::shared_ptr<Player>& player,
	const std::array<const Craft::PixelSprite*, 10>& digitSprites)
{
	if (!canvas || !player)
	{
		return;
	}

	this->player = player;

	// 방향 Sprite 리소스 생성
	// 모든 스트라타젬 Row에서 같은 Sprite를 사용하므로 한 번만 생성
	upSprite = CreateArrowSprite(StratagemCommand::Up);
	downSprite = CreateArrowSprite(StratagemCommand::Down);
	leftSprite = CreateArrowSprite(StratagemCommand::Left);
	rightSprite = CreateArrowSprite(StratagemCommand::Right);

	const StratagemSystem& stratagemSystem = player->GetStratagemSystem();

	const std::size_t stratagemCount = stratagemSystem.GetStratagemCount();

	if (stratagemCount == 0)
	{
		return;
	}

	// 가장 긴 Command Sequence 탐색
	// 모든 Row 너비를 동일하게 만들기 위함
	std::size_t maxCommandCount = 0;

	for (std::size_t index = 0; index < stratagemCount; ++index)
	{
		const StratagemData* data = stratagemSystem.GetStratagemData(index);

		if (!data)
		{
			continue;
		}

		maxCommandCount = (std::max)(maxCommandCount, data->commandSequence.size());
	}

	// Command 영역 전체 너비
	int commandAreaWidth = 0;

	if (maxCommandCount > 0)
	{
		commandAreaWidth = static_cast<int>(maxCommandCount) * (CommandWidth + CommandSpacing) - CommandSpacing;
	}

	// Row 전체 너비
	rowWidth = CommandOffsetX + commandAreaWidth + CooldownGap + CooldownReservedWidth + RightPadding;

	const Craft::Vector2 initialPosition = Craft::Vector2::Zero;

	const Craft::Vector2 rowSize(rowWidth, RowHeight);

	const Craft::ColorRGB panelColor(20, 20, 20);

	const Craft::ColorRGB borderColor(90, 90, 90);;

	rows.clear();
	rows.reserve(stratagemCount);

	// 등록된 모든 Stratagem에 HUD Row 생성
	for (std::size_t stratagemIndex = 0; stratagemIndex < stratagemCount; ++stratagemIndex)
	{
		const StratagemData* data = stratagemSystem.GetStratagemData(stratagemIndex);

		if (!data)
		{
			continue;
		}

		StratagemHUDRow row;

		row.stratagemIndex = stratagemIndex;

		// Row 배경 Panel
		row.panel = std::make_shared<Craft::HUDPanel>(initialPosition, rowSize, panelColor);

		row.panel->SetSortingOrder(1000);

		canvas->AddWidget(row.panel);

		// Row Border
		row.border = std::make_shared<Craft::HUDBorder>(initialPosition, rowSize, borderColor);

		row.border->SetSortingOrder(1010);

		canvas->AddWidget(row.border);

		// 방향 Command
		row.commandWidgets.reserve(data->commandSequence.size());

		for (const StratagemCommand command : data->commandSequence)
		{
			const Craft::PixelSprite* commandSprite = GetCommandSprite(command);

			if (!commandSprite)
			{
				continue;
			}

			std::shared_ptr<Craft::HUDSprite> widget = std::make_shared<Craft::HUDSprite>(commandSprite, Craft::Vector2::Zero);

			widget->SetSortingOrder(1020);

			canvas->AddWidget(widget);

			row.commandWidgets.push_back(widget);
		}

		// Cooldown 남은 시간
		// 실제 Cooldown 중일 때만 표시
		row.cooldownNumber = std::make_shared<Craft::HUDNumber>(Craft::Vector2::Zero);

		row.cooldownNumber->SetDigitSprites(digitSprites);

		row.cooldownNumber->SetMinDigits(2);

		row.cooldownNumber->SetDigitSpacing(1);

		row.cooldownNumber->SetSortingOrder(1020);

		row.cooldownNumber->SetVisible(false);

		canvas->AddWidget(row.cooldownNumber);

		rows.push_back(std::move(row));
	}

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

	// Player가 사라졌다면 HUD 전체 숨김
	// ========================================================
	if (!ownerPlayer)
	{
		for (StratagemHUDRow& row : rows)
		{
			if (row.panel)
			{
				row.panel->SetVisible(false);
			}

			if (row.border)
			{
				row.border->SetVisible(false);
			}

			for (const auto& widget : row.commandWidgets)
			{
				if (widget)
				{
					widget->SetVisible(false);
				}
			}

			if (row.cooldownNumber)
			{
				row.cooldownNumber->SetVisible(false);
			}
		}

		return;
	}

	const StratagemSystem& stratagemSystem = ownerPlayer->GetStratagemSystem();

	const StratagemState state = stratagemSystem.GetState();

	const StratagemInputResult inputResult = stratagemSystem.GetLastInputResult();

	// 현재까지 입력된 Command 개수
	const std::size_t currentInputIndex = stratagemSystem.GetCurrentInputIndex();

	// HUD 색
	// ========================================================

	// 일반 상태
	const Craft::ColorRGB idleBorderColor(90, 90, 90);

	const Craft::ColorRGB idleColor(105, 105, 105);

	// 입력 후보
	const Craft::ColorRGB inputBorderColor(235, 205, 45);

	const Craft::ColorRGB currentColor(235, 205, 45);

	// 이미 맞게 입력한 Command
	const Craft::ColorRGB completedColor(65, 210, 90);

	// 최종 성공
	const Craft::ColorRGB readyBorderColor(65, 210, 90);

	const Craft::ColorRGB readyColor(70, 230, 100);

	// 잘못된 Sequence
	const Craft::ColorRGB failedBorderColor(220, 55, 55);

	const Craft::ColorRGB failedColor(220, 55, 55);

	// Cooldown
	const Craft::ColorRGB cooldownBorderColor(80, 140, 220);

	const Craft::ColorRGB cooldownColor(80, 140, 220);

	// 입력 후보에서 탈락한 Stratagem
	const Craft::ColorRGB eliminatedBorderColor(45, 45, 45);

	const Craft::ColorRGB eliminatedColor(45, 45, 45);

	// Stratagem 단위 처리
	// ========================================================
	for (StratagemHUDRow& row : rows)
	{
		const std::size_t index = row.stratagemIndex;

		const bool isCandidate = stratagemSystem.IsStratagemCandidate(index);

		const bool isMatched = stratagemSystem.IsMatchedStratagem(index);

		const bool isFeedbackTarget = stratagemSystem.IsFeedbackTarget(index);

		const bool isOnCooldown = stratagemSystem.IsOnCooldown(index);

		// 기본 표시
		// ----------------------------------------------------
		if (row.panel)
		{
			row.panel->SetVisible(true);
		}

		if (row.border)
		{
			row.border->SetVisible(true);
		}

		// Cooldown 숫자
		// ====================================================
		if (row.cooldownNumber)
		{
			if (isOnCooldown)
			{
				// 소수점 버림
				const float remaining = stratagemSystem.GetCooldownRemaining(index);

				const int displaySeconds = static_cast<int>(std::ceil(remaining));

				row.cooldownNumber->SetValue(displaySeconds);

				row.cooldownNumber->SetVisible(true);
			}
			else
			{
				row.cooldownNumber->SetVisible(false);
			}
		}

		// Border 상태
		// ====================================================

		// 완전한 Command를 입력했지만
		// 해당 Stratagem이 Cooldown이었다.
		if (inputResult == StratagemInputResult::Cooldown && isFeedbackTarget)
		{
			row.border->SetBorderColor(cooldownBorderColor);
		}

		// ReadyToThrow로 최종 선택된 Stratagem
		else if (state == StratagemState::ReadyToThrow && isMatched)
		{
			row.border->SetBorderColor(readyBorderColor);
		}

		// 입력 중이며 아직 후보로 살아있음
		else if (state == StratagemState::Inputting && isCandidate)
		{
			row.border->SetBorderColor(inputBorderColor);
		}

		// 입력 중 후보에서 탈락
		else if (state == StratagemState::Inputting)
		{
			row.border->SetBorderColor(eliminatedBorderColor);
		}

		// 아무 Stratagem도 맞지 않은 입력
		else if (inputResult ==StratagemInputResult::Failed)
		{
			row.border->SetBorderColor(failedBorderColor);
		}

		// 평상시에도 Cooldown 중이면
		// 해당 Row를 파란색 Border로 표시
		else if (isOnCooldown)
		{
			row.border->SetBorderColor(cooldownBorderColor);
		}

		else
		{
			row.border->SetBorderColor(idleBorderColor);
		}

		// Command 화살표 상태
		// ====================================================
		for (std::size_t commandIndex = 0; commandIndex < row.commandWidgets.size(); ++commandIndex)
		{
			const std::shared_ptr<Craft::HUDSprite>& widget = row.commandWidgets[commandIndex];

			if (!widget)
			{
				continue;
			}

			widget->SetVisible(true);

			// Cooldown 때문에 사용 실패한 바로 그 Stratagem
			// -----------------------------------------------
			if (inputResult == StratagemInputResult::Cooldown && isFeedbackTarget)
			{
				widget->SetTintColor(cooldownColor);

				continue;
			}

			// 최종 매칭 성공
			// -----------------------------------------------
			if (state == StratagemState::ReadyToThrow)
			{
				if (isMatched)
				{
					widget->SetTintColor(readyColor);
				}
				else
				{
					widget->SetTintColor(eliminatedColor);
				}

				continue;
			}

			// 현재 Command 입력 중
			// -----------------------------------------------
			if (state == StratagemState::Inputting)
			{
				// 이 Row는 더 이상 입력 후보가 아님
				if (!isCandidate)
				{
					widget->SetTintColor(eliminatedColor);

					continue;
				}

				// 이미 정확히 입력한 부분
				if (commandIndex < currentInputIndex)
				{
					widget->SetTintColor(completedColor);

					continue;
				}

				// 지금 입력해야 하는 다음 Command
				if (commandIndex == currentInputIndex)
				{
					widget->SetTintColor(currentColor);

					continue;
				}

				// 아직 입력하지 않은 뒤쪽 Command
				widget->SetTintColor(idleColor);

				continue;
			}

			// 잘못된 Sequence
			// -----------------------------------------------
			if (inputResult == StratagemInputResult::Failed)
			{
				widget->SetTintColor(failedColor);

				continue;
			}

			// 평상시 Cooldown 표시
			// -----------------------------------------------
			if (isOnCooldown)
			{
				widget->SetTintColor(cooldownColor);

				continue;
			}

			// 일반 Idle
			// -----------------------------------------------
			widget->SetTintColor(idleColor);
		}
	}
}

void StratagemHUD::UpdateLayout(const HUDLayoutContext& context)
{
	if (rows.empty())
	{
		return;
	}

	// Viewport 오른쪽이 HUD 영역 시작점
	// ========================================================
	const Craft::Vector2 firstRowPosition(context.viewportSize.x + RightAreaLeftMargin, TopMargin);

	// HUD 전체 높이
	const int totalHeight = static_cast<int>(rows.size()) * RowHeight + static_cast<int>(rows.size() - 1) * RowSpacing;

	// ScreenBuffer 영역 초과 방어
	// ========================================================
	if (firstRowPosition.x + rowWidth > context.screenSize.x)
	{
		return;
	}

	if (firstRowPosition.y + totalHeight > context.screenSize.y)
	{
		return;
	}

	// Stratagem Row 배치
	// ========================================================
	for (std::size_t rowIndex = 0; rowIndex < rows.size(); ++rowIndex)
	{
		StratagemHUDRow& row = rows[rowIndex];

		const int rowY = firstRowPosition.y + static_cast<int>(rowIndex) * (RowHeight + RowSpacing);

		const Craft::Vector2 rowPosition(firstRowPosition.x, rowY);

		if (row.panel)
		{
			row.panel->SetPosition(rowPosition);
		}

		if (row.border)
		{
			row.border->SetPosition(rowPosition);
		}

		// Command 위치
		// ====================================================
		for (std::size_t commandIndex = 0; commandIndex < row.commandWidgets.size(); ++commandIndex)
		{
			const auto& widget = row.commandWidgets[commandIndex];

			if (!widget)
			{
				continue;
			}

			const int commandX = rowPosition.x + CommandOffsetX + static_cast<int>(commandIndex) * (CommandWidth + CommandSpacing);

			const int commandY = rowPosition.y + CommandOffsetY;

			widget->SetPosition(Craft::Vector2(commandX, commandY));
		}

		// Cooldown 숫자는 모든 Row의 오른쪽 끝에 정렬
		// ====================================================
		if (row.cooldownNumber)
		{
			const int cooldownX = rowPosition.x + rowWidth - RightPadding - CooldownReservedWidth;

			const int cooldownY = rowPosition.y + CommandOffsetY;

			row.cooldownNumber->SetPosition(Craft::Vector2(cooldownX, cooldownY));
		}
	}
}
