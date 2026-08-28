#pragma once

#include <Core/Core.h>
#include <Math/Vector2.h>

namespace Craft
{
	class CRAFT_API Camera
	{
	public:
		Camera(const Vector2& viewportSize);
		~Camera() = default;

	public:
		// 카메라의 왼쪽 위 월드 좌표 설정
		void SetPosition(const Vector2& newPosition);

		// 특정 월드 위치가 화면 중앙에 오도록 카메라 이동
		void CenterOn(const Vector2& targetPosition);

		// 월드 좌표 -> 화면 좌표로 변환
		Vector2 WorldToScreen(const Vector2& worldPosition) const;

		// Getter
		inline Vector2 GetPosition() const { return position; }

		inline Vector2 GetViewportSize() const { return viewportSize; }

	private:
		// 카메라가 바라보고 있는 영역의 왼쪽 위 월드 좌표
		Vector2 position = Vector2::Zero;

		// 실제 렌더러가 출력하는 화면 크기
		Vector2 viewportSize = Vector2::Zero;
	};
}