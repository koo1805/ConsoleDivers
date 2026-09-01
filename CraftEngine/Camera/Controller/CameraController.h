#pragma once

#include <Core/Core.h>
#include <Math/Vector2F.h>

namespace Craft
{
	// 전방 선언
	class Camera;

	enum class CameraFollowState
	{
		SmoothFollow,
		LockedFollow,
		DeadZone
	};

	class CRAFT_API CameraController
	{
	public:
		CameraController(Camera& camera);
		~CameraController() = default;

	public:
		// 추적할 월드 위치 설정
		void SetTargetPosition(const Vector2F& newTargetPosition);

		// 목표 움직임 상태 설정
		void SetTargetMoving(bool isMoving);

		// 매 프레임 카메라 이동
		void Tick(float deltaTime);

		// 즉시 목표 중앙으로 이동
		void SnapToTarget();

		// 카메라 보간 상태 설정
		void StartSmoothFollow();

		// 카메라 추적 속도 설정
		void SetFollowSpeed(float newFollowSpeed);

		// Getter
		inline Vector2F GetTargetPosition() const { return targetPosition; }
		inline float GetFollowSpeed() const { return followSpeed; }

	private:
		// 카메라 상태 업데이트 함수
		void UpdateSmoothFollow(float deltaTime);
		void UpdateLockedFollow(float deltaTime);
		void UpdateDeadZone();

		// 목표 데드존 유무 확인 함수
		bool IsTargetInsideDeadZone() const;

	private:
		Camera& camera;

		Vector2F targetPosition = Vector2F::Zero;

		// 카메라 추적 상태
		CameraFollowState followState = CameraFollowState::LockedFollow;

		bool isTargetMoving = false;

		// 카메라가 목표에 따라가는 속도 [값이 클수록 빨라짐]
		float followSpeed = 20.0f;

		// 현재 카메라 추적 속도
		float currentFollowSpeed = 0.0f;

		// 초당 증가하는 추적 속도
		float followAcceleration = 200.0f;

		// 최대 추적 속도
		float maxFollowSpeed = 1200.0f;

		// 보간 종료 거리 값
		float lockTreshold = 0.5f;

		// 데드존 활성화 시간
		float stopTimer = 0.0f;
		float deadZoneActivationDelay = 0.15f;

		// 화면 중앙 기준 데드존 크기
		float deadZoneWidth = 40.0f;
		float deadZoneHeight = 20.0f;
	};
}