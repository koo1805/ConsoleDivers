#include <Engine/Engine.h>
#include <Level/GameLevel.h>
#include <iostream>

int main()
{
	std::cin.get();
	
	// 엔진 객체 생성 및 실행
	Craft::Engine engine;
	engine.AddNewLevel<GameLevel>();
	engine.Run();
}