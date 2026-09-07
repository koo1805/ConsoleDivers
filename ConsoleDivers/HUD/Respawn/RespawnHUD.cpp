#include "RespawnHUD.h"
#include <Render/Sprite/PixelSprite.h>
#include <HUD/Canvas/HUDCanvas.h>
#include <HUD/Number/HUDNumber.h>

#include <cmath>

void RespawnHUD::Initialize(const std::shared_ptr<Craft::HUDCanvas>& canvas, const std::array<const Craft::PixelSprite*, 10>& digitSprites)
{
	if (!canvas)
	{
		return;
	}

	countNumber = std::make_shared<Craft::HUDNumber>(Craft::Vector2::Zero);
	countNumber->SetDigitSprites(digitSprites);

	countNumber->SetMinDigits(1);

	countNumber->SetDigitSpacing(0);

	countNumber->SetSortingOrder(sortingOrder);

	countNumber->SetVisible(false);

	canvas->AddWidget(countNumber);

	isVisible = false;
}

void RespawnHUD::UpdateLayout(const HUDLayoutContext& context)
{
	if (!countNumber)
	{
		return;
	}

	const int numberX = context.viewportSize.x + 5;
	const int numberY = context.viewportSize.y / 2;

	countNumber->SetPosition(Craft::Vector2(numberX, numberY));
}

void RespawnHUD::Show()
{
	if (!countNumber)
	{
		return;
	}

	isVisible = true;

	countNumber->SetVisible(true);
}

void RespawnHUD::Hide()
{
	if (!countNumber)
	{
		return;
	}

	isVisible = false;

	countNumber->SetVisible(false);
}

void RespawnHUD::SetRemainingTime(float remainingTime)
{
	if (!countNumber)
	{
		return;
	}

	int displayTime = static_cast<int>(std::ceil(remainingTime));

	if (displayTime < 1)
	{
		displayTime = 1;
	}

	countNumber->SetValue(displayTime);
}
