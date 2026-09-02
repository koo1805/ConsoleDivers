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

		currentFollowSpeed = followSpeed;

		followState = CameraFollowState::LockedFollow;
	}

	// 카메라 보간 추적 상태 변경 - Smooth
	void CameraController::StartSmoothFollow()
	{
		stopTimer = 0.0f;

		// Smooth 시작 속도 초기화
		currentFollowSpeed = followSpeed;

		followState = CameraFollowState::SmoothFollow;
	}

	void CameraController::SetFollowSpeed(float newFollowSpeed)
	{
		if (newFollowSpeed < 0.0f)
		{
			newFollowSpeed = 0.0f;
		}

		followSpeed = newFollowSpeed;

		// 현재 Smooth 상태가 아니라면 다음 Smooth 시작값도 같이 맞춤
		if (followState != CameraFollowState::SmoothFollow)
		{
			currentFollowSpeed = followSpeed;
		}
	}

	void CameraController::UpdateSmoothFollow(float deltaTime)
	{
		// Smooth 상태
		const Vector2F desiredPosition = camera.CalculateCenterPosition(targetPosition);

		const Vector2F currentPosition = camera.GetPosition();

		// 목표와 현재 위치 차이
		const float differenceX = desiredPosition.x - currentPosition.x;

		const float differenceY = desiredPosition.y - currentPosition.y;

		const float distance = std::sqrt((differenceX * differenceX) + (differenceY * differenceY));

		// 목표와 가까우면 보간 종료
		if (distance <= lockTreshold)
		{
			camera.SetPosition(desiredPosition);

			currentFollowSpeed = followSpeed;

			followState = CameraFollowState::LockedFollow;

			return;
		}

		// Smooth 상태가 유지될수록 추적 속도 증가
		currentFollowSpeed += followAcceleration * deltaTime;
		currentFollowSpeed = (std::min)(currentFollowSpeed, maxFollowSpeed);

		// 이번 프레임 카메라 이동 거리
		const float moveDistance = currentFollowSpeed * deltaTime;

		// 다음 이동에서 목표를 지나칠 것 같으면 목표 위치에 바로 고정
		if (moveDistance >= distance)
		{
			camera.SetPosition(desiredPosition);

			currentFollowSpeed = followSpeed;

			followState = CameraFollowState::LockedFollow;

			return;
		}

		// 목표 방향 계산
		const float directionX = differenceX / distance;
		const float directionY = differenceY / distance;

		// 현재 속도만큼 목표 방향으로 이동
		Vector2F newPosition = currentPosition;

		newPosition.x += directionX * moveDistance;
		newPosition.y += directionY * moveDistance;

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

		// 목표를 화면 중앙에 두기 위해 카메라가 실제로 도달 가능한 위치 계산
		const Vector2F desiredPosition = camera.CalculateCenterPosition(targetPosition);

		const Vector2F currentPosition = camera.GetPosition();

		// 현재 카메라 위치와 실제 도달 가능한 목표 위치의 차이
		const float diffX = desiredPosition.x - currentPosition.x;
		const float diffY = desiredPosition.y - currentPosition.y;

		const float distance = std::sqrt((diffX * diffX) + (diffY * diffY));

		// 목표가 데드존 밖에 있지만 카메라가 이미 월드 끝에 도달한 경우 SmoothFollow로 진입을 안함
		if (distance <= lockTreshold)
		{
			return;
		}

		// 이동 가능할 때 보간 추적 시
		StartSmoothFollow();
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