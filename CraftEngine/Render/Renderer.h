#pragma once

#include <Core/Core.h>
#include <Math/Vector2.h>
#include <Math/ColorRGB.h>
#include <string>
#include <vector>
#include <memory>

namespace Craft
{
	// 전방 선언
	class ScreenBuffer;
	class Sprite;
	class PixelSprite;
	class Camera;

	enum class RenderType
	{
		Text,
		PixelSprite
	};

	// 그리기 기능을 전담하는 전문 객체
	class CRAFT_API Renderer
	{
		// 화면에 그릴 데이터를 명령 단위로 저장하기 위한 구조체
		struct RenderCommand
		{
			// 랜더링 데이터 종류
			RenderType renderType = RenderType::Text;

			// 일반 문자열 랜더링에 사용할 문자값
			std::string image;

			// PixelSprite 랜더링
			const PixelSprite* pixelSprite = nullptr;

			// 위치
			Vector2 position = Vector2::Zero;

			// 문자 전경 색상
			ColorRGB foregroundColor = ColorRGB(255, 255, 255);

			// 문자 배경 색상
			ColorRGB backgroundColor = ColorRGB(0, 0, 0);

			// 그리기 정렬 순서 - 값이 크면 우선순위가 높음
			int sortingOrder = -1;
		};

	public:
		Renderer(const Vector2& screenSize);
		~Renderer();

		// 화면에 그릴 데이터를 제출(전달)하는 함수
		// 일반 문자열
		void Submit(
			const std::string& image,
			const Vector2& position,
			const ColorRGB& foregroundColor = ColorRGB(255, 255, 255),
			const ColorRGB& backgroundColor = ColorRGB(0, 0, 0),
			int sortingOrder = 0
		);
		
		void Submit(
			const Sprite& sprite,
			const Vector2& position,
			const ColorRGB& foregroundColor = ColorRGB(255, 255, 255),
			const ColorRGB& backgroundColor = ColorRGB(0, 0, 0),
			int sortingOrder = 0
		);
		
		// PixelSprite
		void Submit(
			const PixelSprite& sprite,
			const Vector2& position,
			int sortingOrder = 0
		);

		// Draw 이벤트 함수 - Engine에서 호출
		void Draw();

		// 전역 접근 함수
		static Renderer& Get();

		// 카메라 접근 함수
		Camera& GetCamera();

	private:
		// 그리기 작업을 시작할 때 화면 버퍼를 초기화하는 함수
		void Clear();

		// 전달 받은 렌더 명령을 활용해 ScreenBuffer의 Cell 데이터를 구성하는 함수
		void DrawRenderQueue();

		// 완성된 화면 버퍼를 화면에 출력하는 함수
		void Present();

		// 일반 문자열 랜더링
		void RenderText(const RenderCommand& command);

		// PixelSprite 랜더링
		void RenderPixel(const RenderCommand& command);

		// Getter.
		ScreenBuffer* GetCurrentBuffer();

		// 이전 프레임 화면 버퍼 반환
		ScreenBuffer* GetPreviousBuffer();

		// 디버깅
		// CRAFT_RENDERER_DEBUG가 활성화된 경우 Draw()에서 사용
		void DebugRenderer();

		void WriteTerminal(const std::string& output);

	private:
		// 전역 접근이 가능하도록 변수 선언
		static Renderer* instance;

		// 이번 프레임에 그릴 렌더 명령을 모아두는 배열
		// 큐(Queue)처럼 사용
		std::vector<RenderCommand> renderQueue;

		// 화면 크기
		Vector2 screenSize;

		// 각 Cell에 현재 그려진 객체의 sortingOrder를 저장
		// 같은 위치에 여러 객체가 겹칠 경우 그리기 우선순위 판단에 사용
		std::vector<int> sortingOrderBuffer;

		// 이중 버퍼링 구현을 위한 화면 버퍼 2개
		std::unique_ptr<ScreenBuffer> screenBufferArray[2];

		// 버퍼 인덱스
		int currentBufferIndex = 0;

		// 카메라
		std::unique_ptr<Camera> camera;
	};
}