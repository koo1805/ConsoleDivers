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

		const PixelSprite* renderSprite = GetRenderSprite();

		if (sprite == nullptr)
		{
			return;
		}

		if (sprite->IsEmpty())
		{
			return;
		}

		Renderer::Get().SubmitScreenSpace(*renderSprite, position, sortingOrder);
	}

	void HUDSprite::SetSprite(const PixelSprite* newSprite)
	{
		sprite = newSprite;

		if (sprite == nullptr)
		{
			size = Vector2::Zero;

			tintedSprite = PixelSprite();

			return;
		}

		size = Vector2(sprite->GetWidth(), sprite->GetHeight());

		// Tint 사용 중에 Sprite가 변경되었다면 새로운 Sprite 기준으로 Tint 복사본 재생성
		if (hasTintColor)
		{
			RebuildTintedSprite();
		}
	}
	void Craft::HUDSprite::SetTintColor(const ColorRGB& newTintColor)
	{
		tintColor = newTintColor;

		hasTintColor = true;

		// Tint Sprite 즉시 다시 생성
		RebuildTintedSprite();
	}

	void Craft::HUDSprite::ClearTintColor()
	{
		hasTintColor = false;

		// Tint 복사본은 더 이상 사용X
		// 굳이 삭제할 필요는 없지만 상태를 깨끗하게 초기화
		tintedSprite = PixelSprite();
	}

	void Craft::HUDSprite::RebuildTintedSprite()
	{
		if (sprite == nullptr)
		{
			tintedSprite = PixelSprite();

			return;
		}

		const int width = sprite->GetWidth();

		const int height = sprite->GetHeight();

		tintedSprite = PixelSprite(width, height);

		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				const Cell& sourceCell = sprite->GetCell(x, y);

				// '\0'은 투명 Cell
				if (sourceCell.character == '\0')
				{
					continue;
				}

				// 원본 Cell 복사
				Cell tintedCell = sourceCell;

				// 색상 Override
				// 현재 PixelSprite는 대부분 ' ' + foreground/background 동일색 구조이므로
				// 둘 다 Tint
				// ====================================================
				tintedCell.foreground = tintColor;

				tintedCell.background = tintColor;

				tintedSprite.SetCell(x, y, tintedCell);
			}
		}
	}

	const Craft::PixelSprite* Craft::HUDSprite::GetRenderSprite() const
	{
		if (sprite == nullptr)
		{
			return nullptr;
		}

		// Tint가 활성화되어 있으면 원본 대신 Tint 복사본을 출력
		if (hasTintColor)
		{
			return &tintedSprite;
		}

		return sprite;
	}
}