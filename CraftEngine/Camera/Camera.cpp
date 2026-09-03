#include "Camera.h"
#include <algorithm>

namespace Craft
{
	Camera::Camera(const Vector2& viewportSize)
		: viewportSize(viewportSize)
	{ }

	void Camera::SetPosition(const Vector2F& newPosition)
	{
		position = newPosition;

		// 직접 위치를 설정한 경우에도 월드 바깥으로 나가지 않도록 제한
		ClampPosition();
	}

	void Camera::SetCameraClampSize(const Vector2& newWorldSize)
	{
		worldSize = newWorldSize;

		// 월드 크기가 변경되었을때 기존 카메라가 새로운 범위를 벗어날수 있으니 제한
		ClampPosition();
	}

	void Camera::CenterOn(const Vector2F& targetPosition)
	{
		// 대상의 위치가 화면 중앙에 오도록 함
		SetPosition(CalculateCenterPosition(targetPosition));
	}

	Vector2F Camera::CalculateCenterPosition(const Vector2F& targetPosition) const
	{
		Vector2F centerPosition;

		// 목표가 화면 중앙에 오도록 카메라 좌상단 좌표 계산 
		centerPosition.x = targetPosition.x - (static_cast<float>(viewportSize.x) / 2.0f);
		centerPosition.y = targetPosition.y - (static_cast<float>(viewportSize.y) / 2.0f);

		// 카메라 이동 가능한 최대 위치
		const float maxX = static_cast<float>((std::max)(0, worldSize.x - viewportSize.x));
		const float maxY = static_cast<float>((std::max)(0, worldSize.y - viewportSize.y));

		// 실제 도달 가능한 범위로 제한
		centerPosition.x = std::clamp(centerPosition.x, 0.0f, maxX);
		centerPosition.y = std::clamp(centerPosition.y, 0.0f, maxY);

		return centerPosition;
	}

	Vector2 Camera::WorldToScreen(const Vector2F& worldPosition) const
	{
		// 월드 위치에서 현재 카메라 이치를 빼면 실제 화면에 출력할 좌표가 됨
		const Vector2F screenPosition = worldPosition - position;

		// 랜더 좌표 변환 값 반환
		return Vector2(static_cast<int>(screenPosition.x), static_cast<int>(screenPosition.y));
	}
	void Camera::ClampPosition()
	{
		// 카메라 범위 최대치 = 월드 크기 - 뷰포트 크기
		const float maxX = static_cast<float>((std::max)(0, worldSize.x - viewportSize.x));
		const float maxY = static_cast<float>((std::max)(0, worldSize.y - viewportSize.y));

		// 카메라 범위 제한
		position.x = std::clamp(position.x, 0.0f, maxX);
		position.y = std::clamp(position.y, 0.0f, maxY);
	}
}