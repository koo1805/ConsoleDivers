#include "DebugRenderer.h"
#include <Render/Renderer.h>
#include <Render/Cell.h>
#include <Render/Sprite/PixelSprite.h>

namespace Craft
{
	void DebugRenderer::BeginFrame()
	{
		// 이전 프레임 명령 제거
		debugCommand.clear();

		// 이전 프레임 Draw가 끝났으니 DebugSprite제거 가능
		frameSprites.clear();
	}

	void DebugRenderer::FillScreenRect(const Vector2& position, const Vector2& size, const ColorRGB& color, int sortingOrder)
	{
		AddRectCommand(position, size, color, sortingOrder, true);
	}

	void DebugRenderer::FillWorldRect(const Vector2& position, const Vector2& size, const ColorRGB& color, int sortingOrder)
	{
		AddRectCommand(position, size, color, sortingOrder, false);
	}

	void DebugRenderer::DrawScreenRect(const Vector2& position, const Vector2& size, const ColorRGB& color, int sortingOrder)
	{
		// 예외 처리
		if (size.x <= 0 || size.y <= 0)
		{
			return;
		}

		// 위
		FillScreenRect(position, Vector2(size.x, 1), color, sortingOrder);

		// 아래
		if (size.y > 1)
		{
			FillScreenRect(Vector2(position.x, position.y + size.y - 1), Vector2(size.x, 1), color, sortingOrder);
		}

		// 좌 | 우
		if (size.y > 2)
		{
			FillScreenRect(Vector2(position.x, position.y + 1), Vector2(1, size.y - 2), color, sortingOrder);

			if (size.x > 1)
			{
				FillScreenRect(Vector2(position.x + size.x - 1, position.y + 1), Vector2(1, size.y - 2), color, sortingOrder);
			}
		}
	}

	void DebugRenderer::DrawWorldRect(const Vector2& position, const Vector2& size, const ColorRGB& color, int sortingOrder)
	{
		// 예외 처리
		if (size.x <= 0 || size.y <= 0)
		{
			return;
		}

		// 위
		FillWorldRect(position, Vector2(size.x, 1), color, sortingOrder);

		// 아래
		if (size.y > 1)
		{
			FillWorldRect(Vector2(position.x, position.y + size.y - 1), Vector2(size.x, 1), color, sortingOrder);
		}

		// 좌 | 우
		if (size.y > 2)
		{
			FillWorldRect(Vector2(position.x, position.y + 1), Vector2(1, size.y - 2), color, sortingOrder);

			if (size.x > 1)
			{
				FillWorldRect(Vector2(position.x + size.x - 1, position.y + 1), Vector2(1, size.y - 2), color, sortingOrder);
			}
		}
	}

	void DebugRenderer::DebugFlush()
	{
		for (const DebugDrawCommand& command : debugCommand)
		{
			frameSprites.emplace_back(command.size.x, command.size.y);

			PixelSprite& debugSprite = frameSprites.back();

			Cell debugCell;

			debugCell.character = ' ';
			debugCell.foreground = command.color;
			debugCell.background = command.color;

			for (int y = 0; y < command.size.y; ++y)
			{
				for (int x = 0; x < command.size.x; ++x)
				{
					debugSprite.SetCell(x, y, debugCell);
				}
			}

			if (command.screenSpace)
			{
				Renderer::Get().SubmitScreenSpace(debugSprite, command.position, command.sortingOrder);
			}
			else
			{
				Renderer::Get().Submit(debugSprite, command.position, command.sortingOrder);
			}
		}

		// 명령 제출 완료
		// Sprite는 살아있어야 하므로 frameSprite는 지우지 않음
		debugCommand.clear();
	}

	DebugRenderer& DebugRenderer::Get()
	{
		static DebugRenderer instance;

		return instance;
	}

	void DebugRenderer::AddRectCommand(const Vector2& position, const Vector2& size, const ColorRGB& color, int sortingOrder, bool screenSpace)
	{
		// 예외 처리
		if (size.x <= 0 || size.y <= 0)
		{
			return;
		}

		DebugDrawCommand command;

		command.position = position;
		command.size = size;
		command.color = color;
		command.sortingOrder = sortingOrder;
		command.screenSpace = screenSpace;

		debugCommand.emplace_back(command);
	}
}