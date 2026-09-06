#pragma once

#include <HUD/HUDWidget.h>
#include <Render/Sprite/PixelSprite.h>

namespace Craft
{
	// PixelSprite를 HUD 위치에 출력하는 Widget
	// 실제 Sprite 리소스는 게임 쪽에서 관리하고 HUDSprite는 그 Sprite를 참조해서 출력만 담당
	// ============================================================
	class CRAFT_API HUDSprite : public HUDWidget
	{
	public:
		HUDSprite() = default;

		HUDSprite(const PixelSprite* sprite, const Vector2& position);

	public:
		virtual void Draw() override;

		void SetSprite(const PixelSprite* newSprite);

		// Tint Color 설정
		void SetTintColor(const ColorRGB& newTintColor);

		// Tint 해제
		void ClearTintColor();

		inline const PixelSprite* GetSprite() const { return sprite; }

		inline bool HasTintColor() const { return hasTintColor; }

	private:
		// Tint Sprite 다시 생성
		void RebuildTintedSprite();

		// 실제 Draw에 사용할 Sprite 반환
		const PixelSprite* GetRenderSprite() const;

	private:
		// Sprite 자체를 복사하지 않고 참조
		// 게임의 WeaponVisual / HUD Resource 등이 실제 PixelSprite의 생명주기를 관리
		const PixelSprite* sprite = nullptr;

		// Tint된 Sprite 복사본
		PixelSprite tintedSprite;

		ColorRGB tintColor = ColorRGB(255, 255, 255);

		bool hasTintColor = false;
	};
}