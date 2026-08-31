#include "TestBG.h"
#include <Render/Cell.h>
#include <Render/Sprite/PixelSprite.h>
#include <Math/ColorRGB.h>
#include <Math/Vector2F.h>

namespace Craft
{
    namespace
    {
        const int worldWidth = 1080;
        const int worldHeight = 960;
    }

    TestBG::TestBG()
        : Actor(Vector2F::Zero)
    {
        sortingOrder = 1;

		BuildBackground();
    }

    Vector2 TestBG::GetWorldSize()
    {
        return Vector2(worldWidth, worldHeight);
    }

    void TestBG::BuildBackground()
    {
		// ============================================================
		// 1. 월드 배경 생성
		// ============================================================
        PixelSprite background(worldWidth, worldHeight);

		// ------------------------------------------------------------
		// 배경 색상
		// ------------------------------------------------------------

		const Craft::ColorRGB darkGray(30, 30, 30);
		const Craft::ColorRGB gray(55, 55, 55);
		const Craft::ColorRGB blue(30, 80, 160);


		Craft::Cell floorCell;
		floorCell.character = ' ';
		floorCell.foreground = darkGray;
		floorCell.background = darkGray;


		Craft::Cell gridCell;
		gridCell.character = ' ';
		gridCell.foreground = gray;
		gridCell.background = gray;


		Craft::Cell markerCell;
		markerCell.character = ' ';
		markerCell.foreground = blue;
		markerCell.background = blue;


		// ============================================================
		// 2. 배경 채우기
		// ============================================================

		for (int y = 0; y < worldHeight; ++y)
		{
			for (int x = 0; x < worldWidth; ++x)
			{
				Craft::Cell cell = floorCell;


				// ----------------------------------------------------
				// 10칸마다 격자선
				// ----------------------------------------------------

				if (x % 10 == 0 || y % 5 == 0)
				{
					cell = gridCell;
				}


				// ----------------------------------------------------
				// 50칸마다 더 눈에 잘 띄는 기준선
				// ----------------------------------------------------

				if (x % 50 == 0 || y % 25 == 0)
				{
					cell = markerCell;
				}


				background.SetCell(x, y, cell);
			}
		}

		ChangePixelSprite(background);
    }
}