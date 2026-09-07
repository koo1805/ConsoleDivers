#include "MainMenuLevel.h"
#include <Engine/Engine.h>
#include <Input/Input.h>
#include <Render/Renderer.h>
#include <Render/Sprite/PixelSprite.h>
#include <Render/Cell.h>
#include <Level/GameLevel.h>

#include <HUD/Canvas/HUDCanvas.h>
#include <HUD/Manager/HUDManager.h>
#include <HUD/Sprite/HUDSprite.h>

#include <Windows.h>
#include <array>
#include <string>
#include <unordered_map>

using namespace Craft;

namespace
{
	// 메뉴 기본 색상
	// ============================================================

	const ColorRGB NormalColor(210, 210, 210);

	const ColorRGB HoverColor(255, 220, 80);

	const ColorRGB TitleColor(220, 220, 220);

	// 5 x 7 Pixel Font
	using FontPattern = std::array<const char*, 7>;

	const std::unordered_map<char, FontPattern> Font =
	{
		{
			'A',
			{
				"01110",
				"10001",
				"10001",
				"11111",
				"10001",
				"10001",
				"10001"
			}
		},

		{
			'C',
			{
				"01111",
				"10000",
				"10000",
				"10000",
				"10000",
				"10000",
				"01111"
			}
		},

		{
			'D',
			{
				"11110",
				"10001",
				"10001",
				"10001",
				"10001",
				"10001",
				"11110"
			}
		},

		{
			'E',
			{
				"11111",
				"10000",
				"10000",
				"11110",
				"10000",
				"10000",
				"11111"
			}
		},

		{
			'G',
			{
				"01111",
				"10000",
				"10000",
				"10111",
				"10001",
				"10001",
				"01111"
			}
		},

		{
			'I',
			{
				"11111",
				"00100",
				"00100",
				"00100",
				"00100",
				"00100",
				"11111"
			}
		},

		{
			'L',
			{
				"10000",
				"10000",
				"10000",
				"10000",
				"10000",
				"10000",
				"11111"
			}
		},

		{
			'M',
			{
				"10001",
				"11011",
				"10101",
				"10101",
				"10001",
				"10001",
				"10001"
			}
		},

		{
			'N',
			{
				"10001",
				"11001",
				"11001",
				"10101",
				"10011",
				"10011",
				"10001"
			}
		},

		{
			'O',
			{
				"01110",
				"10001",
				"10001",
				"10001",
				"10001",
				"10001",
				"01110"
			}
		},

		{
			'Q',
			{
				"01110",
				"10001",
				"10001",
				"10001",
				"10101",
				"10010",
				"01101"
			}
		},

		{
			'R',
			{
				"11110",
				"10001",
				"10001",
				"11110",
				"10100",
				"10010",
				"10001"
			}
		},

		{
			'S',
			{
				"01111",
				"10000",
				"10000",
				"01110",
				"00001",
				"00001",
				"11110"
			}
		},

		{
			'T',
			{
				"11111",
				"00100",
				"00100",
				"00100",
				"00100",
				"00100",
				"00100"
			}
		},

		{
			'U',
			{
				"10001",
				"10001",
				"10001",
				"10001",
				"10001",
				"10001",
				"01110"
			}
		},

		{
			'V',
			{
				"10001",
				"10001",
				"10001",
				"10001",
				"10001",
				"01010",
				"00100"
			}
		}
	};

	// 문자열 -> PixelSprite
	// scale
	// 1 : 버튼
	// 2 : 큰 제목
	// ============================================================
	std::unique_ptr<PixelSprite> CreateTextSprite(
		const std::string& text,
		const ColorRGB& color,
		int scale)
	{
		constexpr int glyphWidth = 5;
		constexpr int glyphHeight = 7;
		constexpr int glyphSpacing = 1;

		// 공백까지 포함한 전체 Sprite 너비 계산
		const int characterWidth = (glyphWidth + glyphSpacing) * scale;

		const int width = static_cast<int>(text.length()) * characterWidth;

		const int height = glyphHeight * scale;

		auto sprite = std::make_unique<PixelSprite>(width, height);

		// 한 글자씩 처리
		for (int characterIndex = 0; characterIndex < static_cast<int>(text.length()); ++characterIndex)
		{
			const char character = text[characterIndex];

			// 공백은 아무것도 그리지 않는다.
			if (character == ' ')
			{
				continue;
			}

			const auto found = Font.find(character);

			// 정의되지 않은 글자는 건너뜀
			if (found == Font.end())
			{
				continue;
			}

			const FontPattern& pattern = found->second;

			// 5 x 7 패턴 순회
			for (int y = 0; y < glyphHeight; ++y)
			{
				for (int x = 0; x < glyphWidth; ++x)
				{
					// 0이면 투명
					if (pattern[y][x] != '1')
					{
						continue;
					}

					// scale 크기만큼 같은 Cell 생성
					for (int scaleY = 0; scaleY < scale; ++scaleY)
					{
						for (int scaleX = 0; scaleX < scale; ++scaleX)
						{
							const int drawX = characterIndex * characterWidth + x * scale + scaleX;

							const int drawY = y * scale + scaleY;

							Cell cell;

							// PixelSprite에서 색 블록처럼 표시
							cell.character = ' ';

							cell.foreground = color;

							cell.background = color;

							sprite->SetCell(drawX, drawY, cell);
						}
					}
				}
			}
		}

		return sprite;
	}
}

// MainMenuLevel
MainMenuLevel::MainMenuLevel() = default;

MainMenuLevel::~MainMenuLevel()
{
	// HUDManager가 Canvas의 shared_ptr을 가지고 있으므로
	// 레벨이 사라질 때 메뉴 Canvas도 반드시 제거
	if (canvas)
	{
		HUDManager::Get().RemoveCanvas(canvas);
	}
}

// OnInitialized
void MainMenuLevel::OnInitialized()
{
	// Level 초기화 상태 설정
	Level::OnInitialized();

	// 메뉴에서 사용할 Sprite 생성
	CreateMenuSprites();

	// HUD 생성 및 배치
	CreateMenuHUD();
}

void MainMenuLevel::Tick(float deltaTime)
{
	Level::Tick(deltaTime);


	Input& input = Input::Get();

	// Hover 판정
	// --------------------------------------------------------
	startHovered = IsMouseOver(startButtonHUD);

	quitHovered = IsMouseOver(quitButtonHUD);

	// Hover 색상 변경
	if (startButtonHUD)
	{
		if (startHovered)
		{
			startButtonHUD->SetTintColor(HoverColor);
		}
		else
		{
			startButtonHUD->SetTintColor(NormalColor);
		}
	}

	if (quitButtonHUD)
	{
		if (quitHovered)
		{
			quitButtonHUD->SetTintColor(HoverColor);
		}
		else
		{
			quitButtonHUD->SetTintColor(NormalColor);
		}
	}

	// 마우스 좌클릭
	if (!input.GetKeyDown(VK_LBUTTON))
	{
		return;
	}

	// START GAME
	if (startHovered)
	{
		// 기존 Engine의 레벨 전환 기능 사용
		Engine::Get().AddNewLevel<GameLevel>();

		return;
	}

	// QUIT GAME
	if (quitHovered)
	{
		// 기존 Engine 종료 기능 사용
		Engine::Get().Quit();

		return;
	}
}

// Draw
void MainMenuLevel::Draw()
{
	// 기본 Level Actor Draw
	super::Draw();

	// 등록된 메뉴 HUD 출력
	HUDManager::Get().Draw();
}

// CreateMenuSprites
// ============================================================
void MainMenuLevel::CreateMenuSprites()
{
	// 제목은 버튼보다 2배 크게 생성
	titleSprite =
		CreateTextSprite(
			"CONSOLE DIVERS",
			TitleColor,
			2);

	// 버튼
	startButtonSprite =
		CreateTextSprite(
			"START GAME",
			NormalColor,
			1);

	quitButtonSprite =
		CreateTextSprite(
			"QUIT GAME",
			NormalColor,
			1);
}

// CreateMenuHUD
// ============================================================
void MainMenuLevel::CreateMenuHUD()
{
	if (!titleSprite || !startButtonSprite || !quitButtonSprite)
	{
		return;
	}

	// HUD 전체 화면 크기
	const Vector2 screenSize = Renderer::Get().GetScreenSize();

	canvas = std::make_shared<HUDCanvas>();

	// 제목
	// --------------------------------------------------------
	const int titleX = (screenSize.x - titleSprite->GetWidth()) / 2;

	const int titleY = screenSize.y / 5;

	titleHUD = std::make_shared<HUDSprite>(titleSprite.get(), Vector2(titleX, titleY));

	titleHUD->SetSortingOrder(1100);

	// START GAME
	// --------------------------------------------------------
	const int startX = (screenSize.x - startButtonSprite->GetWidth()) / 2;

	const int startY = screenSize.y / 2;

	startButtonHUD = std::make_shared<HUDSprite>(startButtonSprite.get(), Vector2(startX, startY));

	startButtonHUD->SetSortingOrder(1100);

	// QUIT GAME
	// --------------------------------------------------------
	const int quitX = (screenSize.x - quitButtonSprite->GetWidth()) / 2;

	const int quitY = startY + startButtonSprite->GetHeight() + 3;


	quitButtonHUD = std::make_shared<HUDSprite>(quitButtonSprite.get(), Vector2(quitX, quitY));

	quitButtonHUD->SetSortingOrder(1100);

	// Canvas 등록
	// --------------------------------------------------------
	canvas->AddWidget(titleHUD);

	canvas->AddWidget(startButtonHUD);

	canvas->AddWidget(quitButtonHUD);

	HUDManager::Get().AddCanvas(canvas);
}

// IsMouseOver
// ============================================================
bool MainMenuLevel::IsMouseOver(const std::shared_ptr<HUDSprite>& widget) const
{
	if (!widget)
	{
		return false;
	}


	const Vector2 mousePosition = Input::Get().GetMousePosition();

	const Vector2 widgetPosition = widget->GetPosition();

	const Vector2 widgetSize = widget->GetSize();

	// HUD의 사각형 영역 내부인지 확인
	return mousePosition.x >= widgetPosition.x
		&& mousePosition.x < widgetPosition.x + widgetSize.x
		&& mousePosition.y >= widgetPosition.y
		&& mousePosition.y < widgetPosition.y + widgetSize.y;
}