#pragma once

#include <Stratagem/Data/StratagemData.h>

#include <cstddef>
#include <limits>
#include <vector>

// 스트라타젬 입력 상태
enum class StratagemState
{
	Idle,

	// Ctrl + 방향키 입력 중
	Inputting,

	// 특정 Stratagem 커맨드와 완전히 일치
	// 다음 LMB는 Weapon Fire가 아니라 Beacon Throw
	ReadyToThrow
};

// 마지막 커맨드 입력 결과
enum class StratagemInputResult
{
	None,
	Failed,
	Cooldown,
	Completed
};

// 스트라타젬별 Runtime 상태
struct StratagemRuntime
{
	StratagemData data;

	// 각 스트라타젬이 독립적으로 관리하는 쿨타임
	float cooldownRemaining = 0.0f;
};

class StratagemSystem
{
public:
	StratagemSystem();

public:
	// StratagemSystem 전체 Update
	void Update(float deltaTime);

	void CancelInput();

	void Reset();

	// ReadyToThrow 상태에서 Beacon Spawn에 성공했을 때 호출
	// 완전히 매칭됐던 Stratagem의 쿨타임만 시작
	void ConsumeMatchedStratagem();

public:
	inline bool IsInputting() const { return state == StratagemState::Inputting; }

	inline bool IsReadyToThrow() const { return state == StratagemState::ReadyToThrow; }

	inline StratagemState GetState() const { return state; }

	inline StratagemInputResult GetLastInputResult() const { return lastInputResult; }

	// 기존 HUD 호환용
	// 이제 별도 currentInputIndex를 관리하지 않고 실제 입력된 command 수가 곧 진행도
	inline std::size_t GetCurrentInputIndex() const { return currentInputSequence.size(); }

	inline std::size_t GetStratagemCount() const { return stratagems.size(); }

	// 현재 완전히 매칭된 Stratagem 반환 | ReadyToThrow가 아니라면 nullptr
	const StratagemData* GetMatchedStratagem() const;

	// 특정 등록 Stratagem
	const StratagemData* GetStratagemData(std::size_t index) const;

	// 현재 입력된 커맨드
	inline const std::vector<StratagemCommand>& GetCurrentInputSequence() const { return currentInputSequence; }

public:
	// Cooldown 조회
	float GetCooldownRemaining(std::size_t index) const;

	float GetCooldownRatio(std::size_t index) const;

	bool IsOnCooldown(std::size_t index) const;

	bool IsStratagemCandidate(std::size_t index) const;

private:
	// 기본 Stratagem 데이터 등록
	void RegisterDefaultStratagems();

	// 하나의 Stratagem 등록
	bool RegisterStratagem(const StratagemData& data);

	// 모든 독립 쿨타임 감소
	void UpdateCooldowns(float deltaTime);

	// Ctrl + 방향키 처리
	void UpdateCommandInput();

private:
	void BeginInput();

	bool TryGetDirectionInput(StratagemCommand& outCommand) const;

	void ProcessCommand(StratagemCommand command);

	// 현재 입력이 특정 커맨드의 앞부분과 일치하는지 검사
	bool IsPrefixMatch(const std::vector<StratagemCommand>& input, const std::vector<StratagemCommand>& pattern) const;

	// 등록 커맨드끼리 모호한 관계인지 검사
	bool IsAmbiguousSequence(const std::vector<StratagemCommand>& lhs, const std::vector<StratagemCommand>& rhs) const;

	// 실패/쿨타임 등 결과를 남기면서 Idle로 복귀
	void ReturnToIdle(StratagemInputResult result);

private:
	// 사전에 등록된 모든 Stratagem
	std::vector<StratagemRuntime> stratagems;

	// 사용자가 Ctrl 이후 지금까지 입력한 커맨드
	std::vector<StratagemCommand> currentInputSequence;

	// 현재 완전히 일치한 Stratagem 인덱스
	// 슬롯을 미리 선택하는 의미가 절대 아님 | 커맨드가 완성됐을 때만 값이 설정
	static constexpr std::size_t InvalidStratagemIndex = (std::numeric_limits<std::size_t>::max)();

	std::size_t matchedStratagemIndex = InvalidStratagemIndex;

	StratagemState state = StratagemState::Idle;

	StratagemInputResult lastInputResult = StratagemInputResult::None;
};