#pragma once

#include <Core/Core.h>
#include <Math/Vector2.h>
#include <Math/ColorRGB.h>
#include <Render/Sprite/PixelSprite.h>

#include <vector>
#include <deque>

namespace Craft
{
	class CRAFT_API DebugRenderer
	{
	private:
		struct DebugDrawCommand
		{
			Vector2 position = Vector2::Zero;
			Vector2 size = Vector2::Zero;

			ColorRGB color = ColorRGB(255, 255, 255);

			int sortingOrder = 100;

			bool screenSpace = false;
		};

	public:
		// 프레임 Debug 렌더링 시작
		void BeginFrame();

		// Screen 기준 fill 사각형
		void FillScreenRect(const Vector2& position, const Vector2& size, const ColorRGB& color, int sortingOrder = 100);
		// World 기준 fill 사각형
		void FillWorldRect(const Vector2& position, const Vector2& size, const ColorRGB& color, int sortingOrder = 100);

		// Screen 기준 테두리 사각형
		void DrawScreenRect(const Vector2& position, const Vector2& size, const ColorRGB& color, int sortingOrder = 100);
		// World 기준 테두리 사각형
		void DrawWorldRect(const Vector2& position, const Vector2& size, const ColorRGB& color, int sortingOrder = 100);

		// 저장된 Debug명령을 Renderer에 제출
		void DebugFlush();

		// 전역 접근
		static DebugRenderer& Get();

	private:
		void AddRectCommand(const Vector2& position, const Vector2& size, const ColorRGB& color, int sortingOrder, bool screenSpace);

	private:
		// Debug 명령
		std::vector<DebugDrawCommand> debugCommand;

		// Renderer는 PixelSprite 포인터 저장하므로 렌더러의 Draw가 끝날 때까지 Sprite가 남아 있어야함
		std::deque<PixelSprite> frameSprites;
	};
}