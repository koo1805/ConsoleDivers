#pragma once

#include <Level/Level.h>
#include <Algorithm/AStar/Navigation/NavigationGrid.h>
#include <Algorithm/QuadTree/QuadTree.h>

#include <memory>

// 전방 선언
class GameHUD;
class Player;
class WorldMap;
class ArcThrower;
class EnemySpawnManager;
class PlayerRespawnSystem;

namespace Craft
{
	class CameraController;
}

class GameLevel : public Craft::Level
{
	TYPE_DECLARATIONS(GameLevel, Level)

public:
	GameLevel();
	virtual ~GameLevel() override;

public:
	// 레벨 초기화
	virtual void OnInitialized() override;

	// 매 프레임 업데이트
	virtual void Tick(float deltaTime) override;

	// 화면 그리기
	virtual void Draw() override;

	void InitializeArcThrower(const std::shared_ptr<ArcThrower>& arcThrower);

private:
	std::shared_ptr<Player> player;

	std::shared_ptr<Craft::CameraController> cameraController;

	std::unique_ptr<WorldMap> worldMap;

	NavigationGrid navigationGrid;

	std::unique_ptr<Craft::QuadTree> quadTree;

	std::unique_ptr<EnemySpawnManager> enemySpawnManager;

	// 게임 HUD
	std::unique_ptr<GameHUD> gameHUD;

	std::unique_ptr<PlayerRespawnSystem> playerRespawnSystem;
};

