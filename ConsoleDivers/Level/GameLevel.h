#pragma once

#include <Level/Level.h>
#include <memory>

class Actor;

class GameLevel : public Craft::Level
{
	TYPE_DECLARATIONS(GameLevel, Level)

public:
	// 레벨 초기화
	virtual void OnInitialized() override;

	// 매 프레임 업데이트
	virtual void Tick(float deltaTime) override;

private:
	// test용
	std::shared_ptr<Craft::Actor> testActor;

};

