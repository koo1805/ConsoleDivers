#include "CameraController.h"
#include <Camera/Camera.h>
#include <algorithm>
#include <cmath>

namespace Craft
{
	CameraController::CameraController(Camera& camera)
		: camera(camera)
	{ }

	void CameraController::SetTargetPosition(const Vector2F& newTargetPosition)
	{
		targetPosition = newTargetPosition;
	}

	void CameraController::SetTargetMoving(bool isMoving)
	{
		isTargetMoving = isMoving;
	}

	void CameraController::Tick(float deltaTime)
	{
		switch (followState)
		{
		// Smooth상태일 때 보간 추적
		case Craft::CameraFollowState::SmoothFollow:
			UpdateSmoothFollow(deltaTime);
			break;
		// Locked상태일 때 화면 중앙 고정
		case Craft::CameraFollowState::LockedFollow:
			UpdateLockedFollow(deltaTime);
			break;
		// 데드존 상태일 때 고정 및 데드존 상태 확인
		case Craft::CameraFollowState::DeadZone:
			UpdateDeadZone();
			break;
		}
	}

	void CameraController::SnapToTarget()
	{
		camera.CenterOn(targetPosition);

		stopTimer = 0.0f;

		followState = CameraFollowState::LockedFollow;
	}

	// 카메라 보간 추적 상태 변경 - Smooth
	void CameraController::StartSmoothFollow()
	{
		stopTimer = 0.0f;

		followState = CameraFollowState::SmoothFollow;
	}

	void CameraController::SetFollowSpeed(float newFollowSpeed)
	{
		if (newFollowSpeed < 0.0f)
		{
			newFollowSpeed = 0.0f;
		}

		followSpeed = newFollowSpeed;
	}

	void CameraController::UpdateSmoothFollow(float deltaTime)
	{
		// Smooth 상태
		// 목표가 화면 중앙에 오게 되었을 때 Camera의 왼쪽 위 좌표를 계산
		const Vector2 viewportSize = camera.GetViewportSize();

		Vector2F desiredPosition;

		desiredPosition.x = targetPosition.x - (static_cast<float>(viewportSize.x) / 2.0f);
		desiredPosition.y = targetPosition.y - (static_cast<float>(viewportSize.y) / 2.0f);

		const Vector2F currentPosition = camera.GetPosition();

		// 목표와 현재 위치 차이
		const float distanceX = std::abs(desiredPosition.x - currentPosition.x);

		const float distanceY = std::abs(desiredPosition.y - currentPosition.y);

		// 목표와 가까우면 보간 종료
		if (distanceX <= lockTreshold && distanceY <= lockTreshold)
		{
			camera.SetPosition(desiredPosition);

			followState = CameraFollowState::LockedFollow;

			return;
		}

		// 아직 멀면 기존 보간 진행
		// 현재 위치에서 목표 위치까지 부드럽게 이동 (보간)
		const float followMove = std::clamp(followSpeed * deltaTime, 0.0f, 1.0f);

		Vector2F newPosition;

		newPosition.x = currentPosition.x + (desiredPosition.x - currentPosition.x) * followMove;
		newPosition.y = currentPosition.y + (desiredPosition.y - currentPosition.y) * followMove;

		camera.SetPosition(newPosition);
	}

	void CameraController::UpdateLockedFollow(float deltaTime)
	{
		camera.CenterOn(targetPosition);

		if (isTargetMoving)
		{
			stopTimer = 0.0f;
			return;
		}

		// 목표 정지 시간 측정
		stopTimer += deltaTime;

		if (stopTimer >= deadZoneActivationDelay)
		{
			stopTimer = 0.0f;

			followState = CameraFollowState::DeadZone;
		}
	}

	void CameraController::UpdateDeadZone()
	{
		// 화면상의 목표가 데드존 안이면 카메라는 고정
		if (IsTargetInsideDeadZone())
		{
			return;
		}

		// 데드존을 벗어나면 다시 보간 추적
		followState = CameraFollowState::SmoothFollow;
	}

	bool CameraController::IsTargetInsideDeadZone() const
	{
		const Vector2 targetScreenPosition = camera.WorldToScreen(targetPosition);

		const Vector2 viewportSize = camera.GetViewportSize();

		const float screenCenterX = static_cast<float>(viewportSize.x) / 2.0f;
		const float screenCenterY = static_cast<float>(viewportSize.y) / 2.0f;

		const float targetX = static_cast<float>(targetScreenPosition.x);
		const float targetY = static_cast<float>(targetScreenPosition.y);

		const float offsetX = targetX - screenCenterX;
		const float offsetY = targetY - screenCenterY;

		return std::abs(offsetX) <= deadZoneWidth && std::abs(offsetY) <= deadZoneHeight;
	}
}