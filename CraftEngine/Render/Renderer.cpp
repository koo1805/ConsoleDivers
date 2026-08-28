#include "Renderer.h"
#include <Render/ScreenBuffer.h>
#include <Render/Cell.h>
#include <Render/ANSIEncoder.h>
#include <Render/Sprite/Sprite.h>
#include <Render/Sprite/PixelSprite.h>
#include <Camera/Camera.h>
#include <cassert>
#include <iostream>

namespace Craft
{
	// static 변수 초기화
	Renderer* Renderer::instance = nullptr;

	Renderer::Renderer(const Vector2& screenSize)
		: screenSize(screenSize)
	{
		// Renderer는 하나만 존재해야 함
		assert(!instance && "instance should be null");
		instance = this;

		camera = std::make_unique<Camera>(screenSize);

		// 전체 Cell개수
		const int bufferCount = screenSize.x * screenSize.y;
		
		// 각 Cell의 그리기 우선순위를 저장할 배열 생성
		sortingOrderBuffer.resize(static_cast<size_t>(bufferCount), -1);

		// 화면 버퍼 구현을 위한 콘솔 버퍼 생성 및 초기화
		screenBufferArray[0] = std::make_unique<ScreenBuffer>(screenSize);
		screenBufferArray[0]->Clear();

		screenBufferArray[1] = std::make_unique<ScreenBuffer>(screenSize);
		screenBufferArray[1]->Clear();
	}

	Renderer::~Renderer()
	{
		instance = nullptr;
	}

	void Renderer::Submit(
		const std::string& image,
		const Vector2& position,
		const ColorRGB& foregroundColor,
		const ColorRGB& backgroundColor,
		int sortingOrder)
	{
		// 렌더 명령 생성 및 값 설정
		RenderCommand command;
		command.renderType = RenderType::Text;
		command.image = image;
		command.position = position;
		command.foregroundColor = foregroundColor;
		command.backgroundColor = backgroundColor;
		command.sortingOrder = sortingOrder;

		// 렌더 큐에 명령 추가
		renderQueue.emplace_back(command);
	}

	void Renderer::Submit(const Sprite& sprite,
		const Vector2& position,
		const ColorRGB& foregroundColor,
		const ColorRGB& backgroundColor,
		int sortingOrder)
	{
		// Sprite가 비어있으면 건너뛰기
		if (sprite.IsEmpty())
		{
			return;
		}

		const std::vector<std::string>& lines = sprite.GetLines();

		// 렌더 명령 생성 및 값 설정
		for (int y = 0; y < sprite.GetHeight(); ++y)
		{
			Submit(
				lines[y],
				Vector2(position.x, position.y + y),
				foregroundColor,
				backgroundColor,
				sortingOrder
			);
		}
	}

	void Renderer::Submit(const PixelSprite& sprite,
		const Vector2& position,
		int sortingOrder)
	{
		// 렌더 명령 생성 및 값 설정
		RenderCommand command;
		command.renderType = RenderType::PixelSprite;
		command.pixelSprite = &sprite;
		command.position = position;
		command.sortingOrder = sortingOrder;

		// 렌더 큐에 명령 추가
		renderQueue.emplace_back(command);
	}

	void Renderer::Draw()
	{
		// 현재 사용할 화면 버퍼 초기화
		Clear();

		// 제출된 RenderCommand를 Cell에 기록
		DrawRenderQueue();

		// 완성된 화면 출력
		Present();
	}

	Renderer& Renderer::Get()
	{
		// 어서트
		assert(instance && "instance should not be null");
		return *instance;
	}

	Camera& Renderer::GetCamera()
	{
		assert(camera && "Camera should not be null");
		return *camera;
	}

	void Renderer::Clear()
	{
		// 현재 버퍼의 모든 Cell 초기화
		GetCurrentBuffer()->Clear();

		// 모든 Cell의 sortingOrder 초기화
		std::fill(sortingOrderBuffer.begin(), sortingOrderBuffer.end(), -1);
	}

	void Renderer::DrawRenderQueue()
	{
		// 렌더 큐를 순회하면서 그리기 명령 실행
		for (const RenderCommand& command : renderQueue)
		{
			switch (command.renderType)
			{
			// 일반 문자 랜더링
			case RenderType::Text:
				RenderText(command);
				break;

			// PixelSprite 랜더링
			case RenderType::PixelSprite:
				RenderPixel(command);
				break;
			}
		}

		// 렌더큐 비우기
		renderQueue.clear();
	}

	void Renderer::Present()
	{
		// 현재 완성된 화면 버퍼
		ScreenBuffer* currentBuffer = GetCurrentBuffer();

		// 이전 프레임 버퍼
		ScreenBuffer* previousBuffer = GetPreviousBuffer();

		// 콘솔에 한 번만 출력할 문자열
		std::string output;

		// 어느정도 메모리 확보 - 매번 문자열이 재할당 되는 것을 줄임
		output.reserve(static_cast<size_t>(screenSize.x * screenSize.y * 8));

		// 행 단위로 Previous / Current 비교
		for (int y = 0; y < screenSize.y; ++y)
		{
			int x = 0;

			while (x < screenSize.x)
			{
				const Vector2 position(x, y);

				const Cell& currentCell = currentBuffer->GetCell(position);

				const Cell& previousCell = previousBuffer->GetCell(position);

				// 이전 프레임과 같으면 출력 필요 없음
				if (currentCell == previousCell)
				{
					++x;

					continue;
				}

				// 변경된 구간 시작
				const int runStartX = x;

				// 변경된 Cell이 연속되는 동안 이동
				while (x < screenSize.x)
				{
					const Vector2 comparePosition(x, y);

					const Cell& current = currentBuffer->GetCell(comparePosition);

					const Cell& previous = previousBuffer->GetCell(comparePosition);

					// 같은 Cell을 만나면 Dirty Run 종료
					if (current == previous)
					{
						break;
					}

					++x;
				}

				const int runEndX = x;

				// 변경된 영역의 시작점으로 커서 이동
				output += ANSIEncoder::CursorPosition(runStartX + 1, y + 1);

				// 변경된 Cell만 출력
				for (int drawX = runStartX;	drawX < runEndX; ++drawX)
				{
					const Cell& cell = currentBuffer->GetCell(Vector2(drawX, y));

					output += ANSIEncoder::Encode(cell);
				}

				// 색상 초기화
				output += ANSIEncoder::Reset();
			}
		}
		
		if (!output.empty())
		{
			// 완성된 화면을 콘솔에 한번에 출력
			std::cout << output;
			std::cout.flush();
		}

		// 인덱스 업데이트(갱신)
		// 0 -> 1 -> 0 -> 1 ...
		// 마법의 공식 -> One Minus...
		currentBufferIndex = 1 - currentBufferIndex;
	}

	void Renderer::RenderText(const RenderCommand& command)
	{
		// 그릴 문자값이 없으면 건너뛰기
		if (command.image.empty())
		{
			return;
		}

		// 월드 좌표를 화면 좌표로 변환
		const Vector2 screenPosition = camera->WorldToScreen(command.position);

		// y 위치가 화면을 벗어났으면 건너뛰기
		if (screenPosition.y < 0 || screenPosition.y >= screenSize.y)
		{
			return;
		}

		// 그리려는 문자열 길이 값
		const int length = static_cast<int>(command.image.length());

		// 글자의 시작 위치
		const int startX = screenPosition.x;

		// 글자의 끝 위치
		const int endX = startX + length - 1;

		// x 위치가 화면을 벗어났는지 확인
		if (endX < 0 || startX >= screenSize.x)
		{
			return;
		}

		// 실제 그릴 글자의 위치 구하기
		// 삼항 연산자
		const int visibleStart = startX < 0 ? 0 : startX;
		const int visibleEnd = endX >= screenSize.x ? screenSize.x - 1 : endX;

		// 문자열을 루프 순회하면서 Cell단위로 처리
		for (int x = visibleStart; x <= visibleEnd; ++x)
		{
			// 문자열에서 값을 가져올 때 사용할 인덱스
			const int sourceIndex = x - startX;

			// 화면의 2차원 좌표
			const Vector2 position(x, screenPosition.y);

			// 2차원 좌표를 1차원 배열 인덱스로 변환
			// (y * width) + x
			const int index = (screenPosition.y * screenSize.x) + x;

			// 정렬 순서를 비교해서 그릴지 말지를 판정
			// 이미 그려진 값이 우선순위가 높으면 건너뛰기
			// 같거나 새로 그리려는 값이 우선순위가 높으면 덮어쓰기
			if (sortingOrderBuffer[index] > command.sortingOrder)
			{
				continue;
			}

			// 화면 버퍼에 Cell기록
			GetCurrentBuffer()->SetCell(
				position,
				Cell(
					command.image[sourceIndex],
					command.foregroundColor,
					command.backgroundColor
				)
			);

			// 해당 Cell의 현재 그리기 우선순위 기록
			sortingOrderBuffer[index] = command.sortingOrder;
		}
	}

	void Renderer::RenderPixel(const RenderCommand& command)
	{
		if (command.pixelSprite == nullptr)
		{
			return;
		}

		const PixelSprite& sprite = *command.pixelSprite;

		// 월드 좌표를 화면 좌표로 변환
		const Vector2 screenPosition = camera->WorldToScreen(command.position);

		// 검사 변수
		const int spriteLeft = screenPosition.x;
		const int spriteRight = screenPosition.x + sprite.GetWidth() - 1;

		const int spriteTop = screenPosition.y;
		const int spriteBottom = screenPosition.y + sprite.GetHeight() - 1;
		
		// Sprite 전체가 화면 밖인지 검사
		if (spriteRight < 0 || spriteLeft >= screenSize.x || spriteBottom < 0 || spriteTop >= screenSize.y)
		{
			return;
		}

		// 화면에 보이는 Sprite 내부 범위 계산
		int startX = 0;
		int startY = 0;

		int endX = sprite.GetWidth();
		int endY = sprite.GetHeight();

		// Sprite 왼쪽이 화면 밖에 있는 경우
		// 예) screenPosition.x = -20 이면 0~19번 Cell은 볼 필요 없음 > 20번째부터 시작
		if (screenPosition.x < 0)
		{
			startX = -screenPosition.x;
		}
		
		// Sprite 오른쪽 검사
		if (screenPosition.x + endX > screenSize.x)
		{
			endX = screenSize.x - screenPosition.x;
		}

		// Sprite 위쪽 검사
		if (screenPosition.y < 0)
		{
			startY = -screenPosition.y;
		}

		// Sprite 아래쪽 검사
		if (screenPosition.y + endY > screenSize.y)
		{
			endY = screenSize.y - screenPosition.y;
		}

		// 화면에 보이는 셀만 순회
		for (int y = startY; y < endY; ++y)
		{
			for (int x = startX; x < endX; ++x)
			{
				const Cell& cell = sprite.GetCell(x, y);

				// '\0'은 투명 Cell로 사용
				if (cell.character == '\0')
				{
					continue;
				}

				// Sprite 내부 위치 + Sprite의 화면 위치
				const Vector2 position(screenPosition.x + x, screenPosition.y + y);

				// 2차원 좌표를 1차원 배열 인덱스로 변환
				const int index = (position.y * screenSize.x) + position.x;

				// 정렬 순서를 비교해서 그릴지 말지를 판정
				// 이미 그려진 값이 우선순위가 높으면 건너뛰기
				// 같거나 새로 그리려는 값이 우선순위가 높으면 덮어쓰기
				if (sortingOrderBuffer[index] > command.sortingOrder)
				{
					continue;
				}

				// 화면 버퍼에 PixelSprite가 가지고 있는 Cell데이터를 기록
				GetCurrentBuffer()->SetCell(position, cell);

				// 해당 Cell의 현재 그리기 우선순위 기록
				sortingOrderBuffer[index] = command.sortingOrder;
			}
		}
	}

	ScreenBuffer* Renderer::GetCurrentBuffer()
	{
		return screenBufferArray[currentBufferIndex].get();
	}

	ScreenBuffer* Renderer::GetPreviousBuffer()
	{
		return screenBufferArray[1 - currentBufferIndex].get();
	}
}