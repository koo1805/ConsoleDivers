#include "Camera.h"

namespace Craft
{
	Camera::Camera(const Vector2& viewportSize)
		: viewportSize(viewportSize)
	{ }

	void Camera::SetPosition(const Vector2& newPosition)
	{
		position = newPosition;
	}

	void Camera::CenterOn(const Vector2& targetPosition)
	{
		// 대상의 위치가 화면 중앙에 오도록 함
		// 카메라의 왼쪽위 월드 좌표 계산
		position.x = targetPosition.x - (viewportSize.x / 2);
		position.y = targetPosition.y - (viewportSize.y / 2);
	}

	Vector2 Camera::WorldToScreen(const Vector2& worldPosition) const
	{
		// 월드 위치에서 현재 카메라 이치를 빼면 실제 화면에 출력할 좌표가 됨
		return worldPosition - position;
	}
}