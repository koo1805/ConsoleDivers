#pragma once

#include <Actor/Actor.h>
#include <Math/Vector2F.h>
#include <Render/Sprite/PixelSprite.h>

#include <Stratagem/Data/StratagemData.h>

// 스트라타젬 비콘 상태
// ============================================================
enum class StratagemBeaconState
{
	// Player가 던진 후 이동 중
	Flying,

	// 목표 거리까지 이동해서 바닥에 착지
	Landed,

	// callDelay가 끝나 실제 스트라타젬 호출이 발생한 상태
	Called
};

class StratagemBeacon : public Craft::Actor
{
	TYPE_DECLARATIONS(StratagemBeacon, Actor)

public:
	StratagemBeacon(const Craft::Vector2F& position, const Craft::Vector2F& direction, const StratagemData& stratagemData);

public:
	virtual void Tick(float deltaTime) override;

	// 추후 World 충돌이 실제 Actor 기반으로 구성됐을 때 벽 착지 처리에도 사용할 수 있도록 미리 준비
	virtual void OnCollision(const std::shared_ptr<Craft::Actor>& other) override;

public:
	inline StratagemBeaconState GetBeaconState() const { return state; }

	inline bool IsFlying() const { return state == StratagemBeaconState::Flying; }

	inline bool IsLanded() const { return state == StratagemBeaconState::Landed; }

	inline bool HasCalled() const { return state == StratagemBeaconState::Called; }

	// 비콘이 최종적으로 착지한 월드 위치
	inline Craft::Vector2F GetLandingPosition() const { return landingPosition; }

private:
	// 투척 이동
	void UpdateFlying(float deltaTime);

	// 착지 후 호출 대기 상태 업데이트
	void UpdateLanded(float deltaTime);

	// 호출 완료 후 빛기둥 유지시간 처리
	void UpdateCalled(float deltaTime);

	// 착지 처리
	void Land();

	// 스트라타젬 실제 호출 발생
	void CallStratagem();

	// 스트라타젬 종류별 호출 분기
	void ExecuteStratagem();

	// 방향을 안전하게 정규화
	void SetDirection(const Craft::Vector2F& newDirection);

	// 착지 후 Signal Sprite가 착지 위치에서 위쪽으로 뻗도록 위치를 보정
	void ApplySignalPosition();

	// 임시 비콘 PixelSprite 생성
	Craft::PixelSprite CreateBeaconSprite() const;

	// 착지 후 신호 빛기둥 Sprite
	Craft::PixelSprite CreateSignalSprite(bool bright) const;

private:
	// 현재 비콘 상태
	StratagemBeaconState state = StratagemBeaconState::Flying;

	// 이 비콘이 호출할 스트라타젬 데이터
	StratagemData stratagemData;

	// 정규화된 투척 방향
	Craft::Vector2F direction = Craft::Vector2F::Zero;

	// 비콘이 처음 생성된 위치
	Craft::Vector2F startPosition = Craft::Vector2F::Zero;

	// 착지 위치
	Craft::Vector2F landingPosition = Craft::Vector2F::Zero;

	// 투척 중 이동한 거리
	float traveledDistance = 0.0f;

	// 호출 관련 Timer
	// 착지 후 실제 호출까지 누적 시간
	float callTimer = 0.0f;

	// 착지 후 비콘 이펙트가 유지된 전체 시간
	float beaconTimer = 0.0f;

	// 빛기둥 깜빡임용 Timer
	float signalBlinkTimer = 0.0f;

	// 현재 밝은 Sprite인지
	bool isSignalBright = true;

private:
	// ========================================================
	// 비콘 투척 설정
	//
	// 현재는 테스트 가능한 고정값.
	// 추후 Setting 또는 StratagemData로 이동 가능.
	// ========================================================

	// 월드 셀 기준 초당 이동 속도
	static constexpr float throwSpeed = 80.0f;

	// 최대 투척 거리
	static constexpr float maxThrowDistance = 45.0f;

	// 비콘 신호 깜빡임 주기
	static constexpr float signalBlinkInterval = 0.15f;

	// Signal Beam 크기
	static constexpr int SignalWidth = 3;
	static constexpr int SignalHeight = 20;
};