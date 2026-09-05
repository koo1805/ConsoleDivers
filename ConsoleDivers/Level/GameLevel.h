#pragma once

#include <Level/Level.h>
#include <Algorithm/AStar/Navigation/NavigationGrid.h>
#include <Algorithm/QuadTree/QuadTree.h>

#include <memory>

// 전방 선언
class Player;
class NormalEnemy;

namespace Craft
{
	class TestBG;
	class CameraController;
}

class GameLevel : public Craft::Level
{
	TYPE_DECLARATIONS(GameLevel, Level)

public:
	// 레벨 초기화
	virtual void OnInitialized() override;

	// 매 프레임 업데이트
	virtual void Tick(float deltaTime) override;

	// 화면 그리기
	virtual void Draw() override;

private:
	std::shared_ptr<Player> player;

	std::shared_ptr<NormalEnemy> normalEnemy;
	// test용
	std::shared_ptr<Craft::TestBG> testBGActor;

	std::shared_ptr<Craft::CameraController> cameraController;

	NavigationGrid navigationGrid;

	std::unique_ptr<Craft::QuadTree> quadTree;
};

