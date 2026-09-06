#include "StratagemSystem.h"
#include <Input/Input.h>

#include <algorithm>
#include <Windows.h>

using namespace Craft;

StratagemSystem::StratagemSystem()
{
	RegisterDefaultStratagems();	
}

void StratagemSystem::UpdateCommandInput()
{
	Input& input = Input::Get();

	// 1. Ctrl을 처음 누른 순간 스트라타젬 입력 시작
	// ========================================================
	if (state == StratagemState::Idle)
	{
		if (input.GetKeyDown(VK_CONTROL))
		{
			BeginInput();
		}
	}

	// 입력 중이 아니라면 방향키를 확인할 필요 없음
	if (state != StratagemState::Inputting)
	{
		return;
	}

	// 2. 입력 도중 Ctrl을 놓으면 취소
	// ========================================================
	if (input.GetKeyUp(VK_CONTROL))
	{
		CancelInput();
		return;
	}

	// Ctrl이 실제로 눌린 동안에만 방향 입력 처리
	if (!input.GetKey(VK_CONTROL))
	{
		return;
	}

	StratagemCommand inputCommand;

	// 이번 프레임에 방향 입력이 없다면 종료
	if (!TryGetDirectionInput(inputCommand))
	{
		return;
	}

	// 실제 스트라타젬 패턴과 비교
	ProcessCommand(inputCommand);
}

void StratagemSystem::BeginInput()
{
	// 이미 입력 중이거나 완료 상태라면 다시 시작하지 않음
	if (state != StratagemState::Idle)
	{
		return;
	}

	if (stratagems.empty())
	{
		return;
	}

	// 아직 어떤 Stratagem인지 결정되지 않은 상태
	// 완전한 Command가 일치했을 때 해당 Stratagem의 Cooldown을 검사
	currentInputSequence.clear();

	matchedStratagemIndex = InvalidStratagemIndex;

	// 새 입력을 시작하므로 이전 실패 / 쿨타임 표시 대상 제거
	feedbackStratagemIndex = InvalidStratagemIndex;

	lastInputResult =StratagemInputResult::None;

	state = StratagemState::Inputting;
}

bool StratagemSystem::TryGetDirectionInput(StratagemCommand& outCommand) const
{
	const Input& input = Input::Get();

	if (input.GetKeyDown(VK_UP))
	{
		outCommand = StratagemCommand::Up;
		return true;
	}

	if (input.GetKeyDown(VK_DOWN))
	{
		outCommand = StratagemCommand::Down;
		return true;
	}

	if (input.GetKeyDown(VK_LEFT))
	{
		outCommand = StratagemCommand::Left;
		return true;
	}

	if (input.GetKeyDown(VK_RIGHT))
	{
		outCommand = StratagemCommand::Right;
		return true;
	}

	return false;
}

void StratagemSystem::ProcessCommand(StratagemCommand command)
{
	// 이번 방향 입력을 현재 입력 Sequence 뒤에 추가
	currentInputSequence.push_back(command);

	bool hasPrefixMatch = false;

	std::size_t completedMatchIndex = InvalidStratagemIndex;

	// 등록된 모든 Stratagem 검사
	for (std::size_t index = 0; index < stratagems.size(); ++index)
	{
		const StratagemRuntime& runtime = stratagems[index];

		const std::vector<StratagemCommand>& pattern = runtime.data.commandSequence;

		// 현재 입력이 이 Stratagem 커맨드의 앞부분인지 검사
		if (!IsPrefixMatch(currentInputSequence, pattern))
		{
			continue;
		}

		// 적어도 하나의 Stratagem 후보가 살아있음
		hasPrefixMatch = true;

		// 길이까지 정확히 같으면 완전 일치
		if (currentInputSequence.size() == pattern.size())
		{
			completedMatchIndex = index;

			break;
		}
	}

	// 어떤 Stratagem의 Prefix도 아니다 -> 즉시 실패
	if (!hasPrefixMatch)
	{
		ReturnToIdle(StratagemInputResult::Failed, InvalidStratagemIndex);
		return;
	}

	// 아직 완성된 커맨드는 없음
	// 하지만 최소 하나 이상의 Stratagem Prefix와 일치하고 있으므로 계속 입력을 기다린다.
	if (completedMatchIndex == InvalidStratagemIndex)
	{
		return;
	}

	// 특정 Stratagem과 완전히 일치
	StratagemRuntime& matchedRuntime = stratagems[completedMatchIndex];

	// 커맨드는 맞았지만 해당 Stratagem이 Cooldown 중
	if (matchedRuntime.cooldownRemaining > 0.0f)
	{
		ReturnToIdle(StratagemInputResult::Cooldown, completedMatchIndex);

		return;
	}

	// 완전 성공
	matchedStratagemIndex = completedMatchIndex;

	feedbackStratagemIndex = InvalidStratagemIndex;

	lastInputResult = StratagemInputResult::Completed;

	state = StratagemState::ReadyToThrow;
}

bool StratagemSystem::IsPrefixMatch(const std::vector<StratagemCommand>& input, const std::vector<StratagemCommand>& pattern) const
{
	// 입력이 Pattern보다 더 길면 절대 Prefix가 될 수 없음
	if (input.size() > pattern.size())
	{
		return false;
	}

	for (std::size_t index = 0; index < input.size(); ++index)
	{
		if (input[index] != pattern[index])
		{
			return false;
		}
	}

	return true;
}

bool StratagemSystem::IsAmbiguousSequence(const std::vector<StratagemCommand>& lhs, const std::vector<StratagemCommand>& rhs) const
{
	// 더 짧은 쪽 길이까지만 비교
	// 모든 값이 같다면:
	// 1. 완전히 같은 커맨드거나 2. 한쪽이 다른 쪽의 Prefix -> 둘 다 모호한 구조
	const std::size_t compareCount = (std::min)(lhs.size(), rhs.size());

	for (std::size_t index = 0; index < compareCount; ++index)
	{
		if (lhs[index] != rhs[index])
		{
			// 중간에서 갈라지므로 서로 구별 가능
			return false;
		}
	}

	return true;
}

void StratagemSystem::ReturnToIdle(StratagemInputResult result, std::size_t feedbackIndex)
{
	currentInputSequence.clear();

	matchedStratagemIndex = InvalidStratagemIndex;

	// 실패 결과와 함께 어떤 Stratagem에 대한 결과인지도 저장
	feedbackStratagemIndex = feedbackIndex;

	state = StratagemState::Idle;

	// Failed / Cooldown 등의 결과는 유지
	lastInputResult = result;
}

void StratagemSystem::Update(float deltaTime)
{
	// 1. 모든 Stratagem의 독립 쿨타임 감소
	UpdateCooldowns(deltaTime);

	// 2. Ctrl + 방향키 Command 입력
	UpdateCommandInput();
}

void StratagemSystem::CancelInput()
{
	currentInputSequence.clear();

	matchedStratagemIndex = InvalidStratagemIndex;

	feedbackStratagemIndex = InvalidStratagemIndex;

	// 평상 상태로 복귀
	state = StratagemState::Idle;

	// 일반 취소이므로 실패 표시를 남기지 않음
	lastInputResult = StratagemInputResult::None;
}

void StratagemSystem::Reset()
{
	currentInputSequence.clear();

	matchedStratagemIndex = InvalidStratagemIndex;

	feedbackStratagemIndex = InvalidStratagemIndex;

	state = StratagemState::Idle;

	// 일반 취소이므로 실패 표시를 남기지 않음
	lastInputResult = StratagemInputResult::None;
}

void StratagemSystem::ConsumeMatchedStratagem()
{
	// ReadyToThrow 상태가 아니면 사용 불가
	if (state != StratagemState::ReadyToThrow)
	{
		return;
	}

	if (matchedStratagemIndex == InvalidStratagemIndex)
	{
		return;
	}

	if (matchedStratagemIndex >= stratagems.size())
	{
		return;
	}

	StratagemRuntime& runtime = stratagems[matchedStratagemIndex];

	// 완전히 매칭된 그 Stratagem만 Cooldown 시작
	runtime.cooldownRemaining = runtime.data.cooldown;

	Reset();
}

const StratagemData* StratagemSystem::GetMatchedStratagem() const
{
	if (matchedStratagemIndex == InvalidStratagemIndex)
	{
		return nullptr;
	}

	if (matchedStratagemIndex >= stratagems.size())
	{
		return nullptr;
	}

	return &stratagems[matchedStratagemIndex].data;
}

const StratagemData* StratagemSystem::GetStratagemData(std::size_t index) const
{
	if (index >= stratagems.size())
	{
		return nullptr;
	}

	return &stratagems[index].data;
}

float StratagemSystem::GetCooldownRemaining(std::size_t index) const
{
	if (index >= stratagems.size())
	{
		return 0.0f;
	}

	return stratagems[index].cooldownRemaining;
}

float StratagemSystem::GetCooldownRatio(std::size_t index) const
{
	if (index >= stratagems.size())
	{
		return 0.0f;
	}

	const StratagemRuntime& runtime = stratagems[index];

	if (runtime.data.cooldown <= 0.0f)
	{
		return 0.0f;
	}

	float ratio = runtime.cooldownRemaining / runtime.data.cooldown;

	if (ratio < 0.0f)
	{
		ratio = 0.0f;
	}
	else if (ratio > 1.0f)
	{
		ratio = 1.0f;
	}

	return ratio;
}

bool StratagemSystem::IsOnCooldown(std::size_t index) const
{
	if (index >= stratagems.size())
	{
		return false;
	}

	return stratagems[index].cooldownRemaining > 0.0f;
}

bool StratagemSystem::IsStratagemCandidate(std::size_t index) const
{
	if (index >= stratagems.size())
	{
		return false;
	}

	if (currentInputSequence.empty())
	{
		return true;
	}

	return IsPrefixMatch(currentInputSequence, stratagems[index].data.commandSequence);
}

void StratagemSystem::RegisterDefaultStratagems()
{
	stratagems.clear();
	// Arc Thrower
	StratagemData arcThrower;
	arcThrower.type = StratagemType::SupportWeapon;
	arcThrower.id = StratagemId::ArcThrower;
	arcThrower.name = "Arc Thrower";

	arcThrower.commandSequence =
	{
		StratagemCommand::Down,
		StratagemCommand::Left,
		StratagemCommand::Down,
		StratagemCommand::Up,
		StratagemCommand::Right
	};

	// 아직 실제 사용하지 않는 후속 시스템용 데이터
	arcThrower.cooldown = 10.0f;

	// 비콘 착지 후 실제 효과 호출까지 .초
	arcThrower.callDelay = 2.0f;

	// 비콘 착지 후 신호가 유지되는 총 시간 .초
	arcThrower.beaconDuration = 4.0f;

	RegisterStratagem(arcThrower);

	// ========================================================
	// 이후 실제 Stratagem을 만들 때 여기에 추가
}

bool StratagemSystem::RegisterStratagem(const StratagemData& data)
{
	// ID가 없는 데이터는 등록하지 않음
	if (data.id == StratagemId::None)
	{
		return false;
	}

	// 커맨드가 없는 Stratagem도 등록 불가
	if (data.commandSequence.empty())
	{
		return false;
	}

	// 기존 커맨드와 완전히 같거나, 어느 한쪽이 다른 쪽의 완전한 Prefix이면
	// 입력만으로 구별할 수 없으므로 등록 거부
	for (const StratagemRuntime& runtime : stratagems)
	{
		if (IsAmbiguousSequence(data.commandSequence, runtime.data.commandSequence))
		{
			return false;
		}
	}

	StratagemRuntime runtime;

	runtime.data = data;
	runtime.cooldownRemaining = 0.0f;

	stratagems.push_back(runtime);

	return true;
}

void StratagemSystem::UpdateCooldowns(float deltaTime)
{
	if (deltaTime <= 0.0f)
	{
		return;
	}

	for (StratagemRuntime& runtime : stratagems)
	{
		if (runtime.cooldownRemaining <= 0.0f)
		{
			runtime.cooldownRemaining = 0.0f;
			continue;
		}

		runtime.cooldownRemaining -= deltaTime;

		// 음수 방지
		if (runtime.cooldownRemaining < 0.0f)
		{
			runtime.cooldownRemaining = 0.0f;
		}
	}
}
