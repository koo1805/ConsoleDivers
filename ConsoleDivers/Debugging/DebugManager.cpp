#include "DebugManager.h"
#include <Camera/Camera.h>
#include <Camera/Controller/CameraController.h>
#include <Render/Renderer.h>
#include <Render/Debugging/DebugRenderer.h>
#include <Input/Input.h>

#include <Windows.h>

namespace Craft
{
	DebugManager& DebugManager::Get()
	{
		static DebugManager instance;

		return instance;
	}

	void DebugManager::SetCameraController(CameraController* newCameraController)
	{
		cameraController = newCameraController;

		if (cameraController)
		{
			previousDeadZoneActive = cameraController->IsDeadZoneActive();
		}
		else
		{
			previousDeadZoneActive = false;
		}

		deadZoneExitTimer = 0.0f;
	}

	void DebugManager::Tick(float deltaTime)
	{
		// 토글 형식 디버그 Input
		if (Input::Get().GetKeyDown(VK_F3))
		{
			deadZoneDebugEnabled = !deadZoneDebugEnabled;

			if (!deadZoneDebugEnabled)
			{
				deadZoneExitTimer = 0.0f;
			}
		}

		UpdateDeadZoneDebug(deltaTime);
	}

	void DebugManager::Draw()
	{
		DebugRenderer& debugRenderer = DebugRenderer::Get();

		// 이전 프레임 디버그 데이터 제거
		debugRenderer.BeginFrame();

		DrawDeadZoneDebug();

		// Debug 렌더 명령을 실제 렌더러에게 전달
		debugRenderer.DebugFlush();
	}

	void DebugManager::UpdateDeadZoneDebug(float deltaTime)
	{
		// 예외 처리
		if (!cameraController)
		{
			return;
		}

		// 기존 데드존 이탈 시간 감소
		if (deadZoneExitTimer > 0.0f)
		{
			deadZoneExitTimer -= deltaTime;

			if (deadZoneExitTimer < 0.0f)
			{
				deadZoneExitTimer = 0.0f;
			}
		}

		const bool currentDeadZoneActive = cameraController->IsDeadZoneActive();

		if (deadZoneDebugEnabled)
		{
			// 데드존 이탈 감지
			if (previousDeadZoneActive && !currentDeadZoneActive)
			{
				deadZoneExitTimer = deadZoneExitDuration;
			}
		}

		previousDeadZoneActive = currentDeadZoneActive;
	}

	void DebugManager::DrawDeadZoneDebug()
	{
		if (!deadZoneDebugEnabled)
		{
			return;
		}

		if (!cameraController)
		{
			return;
		}

		const Craft::Vector2 viewportSize = Renderer::Get().GetCamera().GetViewportSize();

		const int deadZoneHalfWidth = static_cast<int>(cameraController->GetDeadZoneWidth());
		const int deadZoneHalfHeight = static_cast<int>(cameraController->GetDeadZoneHeight());

		const Craft::Vector2 deadZoneSize(deadZoneHalfWidth * 2 + 1, deadZoneHalfHeight * 2 + 1);

		const Craft::Vector2 deadZonePosition((viewportSize.x / 2) - deadZoneHalfWidth, (viewportSize.y / 2) - deadZoneHalfHeight);

		// 데드존 이탈 후
		if (deadZoneExitTimer > 0.0f)
		{
			DebugRenderer::Get().FillScreenRect(deadZonePosition, deadZoneSize, Craft::ColorRGB(190, 110, 110), 5);

			return;
		}

		// 데드존 활성 상태
		if (cameraController->IsDeadZoneActive())
		{
			DebugRenderer::Get().FillScreenRect(deadZonePosition, deadZoneSize, Craft::ColorRGB(120, 180, 120), 5);
		}
	}
}