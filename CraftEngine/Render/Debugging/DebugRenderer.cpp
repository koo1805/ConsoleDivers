#include "DebugRenderer.h"
#include <Render/Renderer.h>
#include <Render/Cell.h>
#include <Render/Sprite/PixelSprite.h>

#include <cstdlib>

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

	void DebugRenderer::DrawWorldLine(const Vector2& start, const Vector2& end, const ColorRGB& color, int sortingOrder)
	{
		// Bresenham Line Algorithm
		// float 연산 없이 정수 World Cell들을 따라가면서 시작점과 끝점 사이에 선을 생성

		int x0 = start.x;
		int y0 = start.y;

		const int x1 = end.x;
		const int y1 = end.y;

		// X 방향 이동 거리
		const int deltaX = std::abs(x1 - x0);

		// X 방향 이동 방향
		const int stepX = (x0 < x1) ? 1 : -1;

		// Y 방향 이동 거리
		// Bresenham 계산식에서 음수 값으로 사용
		const int deltaY = -std::abs(y1 - y0);

		// Y 방향 이동 방향
		const int stepY = (y0 < y1) ? 1 : -1;

		// 현재 오차 값
		int error = deltaX + deltaY;

		while (true)
		{
			// 현재 World 위치에 1x1 Debug Cell 생성
			FillWorldRect(Vector2(x0, y0), Vector2(1, 1), color, sortingOrder);

			// 끝점에 도달
			if (x0 == x1 && y0 == y1)
			{
				break;
			}

			const int doubledError = error * 2;

			// X 방향 이동 여부
			if (doubledError >= deltaY)
			{
				error += deltaY;

				x0 += stepX;
			}

			// Y 방향 이동 여부
			if (doubledError <= deltaX)
			{
				error += deltaX;

				y0 += stepY;
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