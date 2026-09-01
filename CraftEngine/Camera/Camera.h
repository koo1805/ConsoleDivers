#pragma once

#include <Core/Core.h>
#include <Math/Vector2.h>
#include <Math/Vector2F.h>

namespace Craft
{
	class CRAFT_API Camera
	{
	public:
		Camera(const Vector2& viewportSize);
		~Camera() = default;

	public:
		// 카메라의 왼쪽 위 월드 좌표 설정
		void SetPosition(const Vector2F& newPosition);

		// 카메라가 이동할 수 있는 월드 설정
		void SetCameraClampSize(const Vector2& newWorldSize);

		// 특정 월드 위치가 화면 중앙에 오도록 카메라 이동
		void CenterOn(const Vector2F& targetPosition);

		// 중앙 정렬 시 실제 도달 가능한 카메라 위치 계산
		Vector2F CalculateCenterPosition(const Vector2F& targetPosition) const;

		// 월드 좌표 -> 화면 좌표로 변환
		Vector2 WorldToScreen(const Vector2F& worldPosition) const;

		// Getter
		inline Vector2F GetPosition() const { return position; }

		inline Vector2 GetViewportSize() const { return viewportSize; }

		inline Vector2 GetClampWorldSize() const { return worldSize; }

	private:
		// 카메라 위치가 월드 범위를 벗어나지 않게 제한
		void ClampPosition();

	private:
		// 카메라가 바라보고 있는 영역의 왼쪽 위 월드 좌표
		Vector2F position = Vector2F::Zero;

		// 실제 렌더러가 출력하는 화면 크기
		Vector2 viewportSize = Vector2::Zero;

		// 카메라가 이동할 수 있는 월드 크기
		Vector2 worldSize = Vector2::Zero;
	};
}