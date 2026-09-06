#pragma once

#include <vector>
#include <string>

// 스트라타젬 방향 입력
enum class StratagemCommand
{
	Up,
	Down,
	Left,
	Right
};

// 스트라타젬 종류
enum class StratagemType
{
	SupportWeapon,
	Sentry,
	Offensive
};

enum class StratagemId
{
	None,

	// 테스트용 첫 번째 지원무기
	ArcThrower
};

// 스트라타젬 하나의 고정 데이터
struct StratagemData
{
	// 어떤 종류의 스트라타젬인지
	StratagemType type = StratagemType::SupportWeapon;

	// 실제 스트라타젬 종류
	StratagemId id = StratagemId::None;

	// HUD 등에 표시할 이름
	std::string name;

	// 입력해야 하는 방향 커맨드
	// 방향키
	std::vector<StratagemCommand> commandSequence;

	// 재사용 대기시간
	// 현재 단계에서는 데이터만 준비하고
	// 실제 쿨타임 처리는 후속 단계에서 구현
	float cooldown = 0.0f;

	// 비콘 착지 후 실제 호출까지 기다리는 시간
	float callDelay = 0.0f;

	// 착지 후 빛기둥이 유지되는 총 시간
	float beaconDuration = 0.0f;
};