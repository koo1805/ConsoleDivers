#include "HUDSprite.h"
#include <Render/Renderer.h>

namespace Craft
{
	HUDSprite::HUDSprite(const PixelSprite* sprite, const Vector2& position)
		: HUDWidget(position), sprite(sprite)
	{
		sortingOrder = 1020;

		// Sprite가 존재하면 자동으로 크기 설정
		if (sprite)
		{
			size = Vector2(sprite->GetWidth(), sprite->GetHeight());
		}
	}

	void HUDSprite::Draw()
	{
		if (!visible)
		{
			return;
		}

		if (sprite == nullptr)
		{
			return;
		}

		if (sprite->IsEmpty())
		{
			return;
		}

		Renderer::Get().SubmitScreenSpace(*sprite, position, sortingOrder);
	}

	void HUDSprite::SetSprite(const PixelSprite* newSprite)
	{
		sprite = newSprite;

		if (sprite == nullptr)
		{
			size = Vector2::Zero;
			return;
		}

		size = Vector2(sprite->GetWidth(), sprite->GetHeight());
	}
}