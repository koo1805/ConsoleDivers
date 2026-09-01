#include "Renderer.h"
#include <Render/ScreenBuffer.h>
#include <Render/Cell.h>
#include <Render/ANSIEncoder.h>
#include <Render/Sprite/Sprite.h>
#include <Render/Sprite/PixelSprite.h>
#include <Camera/Camera.h>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <Windows.h>

// 디버깅
// 프로젝트 전처리기에서 CRAFT_RENDERER_DEBUG를 직접 지정하지 않았다면
// Debug 빌드   -> 1
// Release 빌드 -> 0
// 프로젝트 속성에서  CRAFT_RENDERER_DEBUG=0  으로 지정하면 Debug 빌드에서도 Renderer 성능 측정을 끌 수 있음
#ifndef CRAFT_RENDERER_DEBUG

	#ifdef _DEBUG
	#define CRAFT_RENDERER_DEBUG 1
	#else
	#define CRAFT_RENDERER_DEBUG 0
	#endif

#endif

// Renderer 디버깅에만 필요한 헤더
#if CRAFT_RENDERER_DEBUG
#include <chrono>
#include <sstream>
#endif

namespace Craft
{
	enum class TerminalWriteMode
	{
		StdCout,
		WriteFile
	};

	constexpr TerminalWriteMode TERMINAL_WRITE_MODE = TerminalWriteMode::WriteFile;

	// 정수의 10진수 자릿수
	int DigitCount(int value)
	{
		if (value >= 1000)
		{
			return 4;
		}

		if (value >= 100)
		{
			return 3;
		}

		if (value >= 10)
		{
			return 2;
		}

		return 1;
	}

	// 총 4 + row digits + column digits
	int AbsoluteCursorByteCount(int x, int y)
	{
		return 4 + DigitCount(y) + DigitCount(x);
	}

	// 총 3 + distance digits
	int RelativeCursorByteCount(int distance)
	{
		// ANSI 기본 이동 거리가 1이므로 숫자 생략 가능
		// ESC[C
		if (distance == 1)
		{
			return 3;
		}

		return 3 + DigitCount(distance);
	}

	int CursorDownByteCount(int distance)
	{
		// ANSI 기본 이동 거리가 1이므로 숫자 생략 가능
		if (distance == 1)
		{
			return 3;
		}

		return 3 + DigitCount(distance);
	}

	int CursorColumnByteCount(int column)
	{
		return 3 + DigitCount(column);
	}

	int CursorNextLineByteCount(int distance)
	{
		// ANSI 기본 이동 거리가 1이므로 숫자 생략 가능
		if (distance == 1)
		{
			return 3;
		}

		return 3 + DigitCount(distance);
	}

	// ============================================================
#if CRAFT_RENDERER_DEBUG
	struct RendererDebugStats
	{
		// Present() 관련 카운터
		// 이전 프레임과 달라진 Cell 개수
		size_t dirtyCellCount = 0;

		// 변경된 연속 구간 개수
		size_t dirtyRunCount = 0;

		// ANSI 전경색이 실제 변경된 횟수
		size_t foregroundChangeCount = 0;

		// ANSI 배경색이 실제 변경된 횟수
		size_t backgroundChangeCount = 0;

		// 터미널로 전달한 최종 문자열 크기
		size_t outputByteCount = 0;

		// 절대 좌표 CursorPosition을 사용한 횟수
		size_t absoluteCursorCount = 0;

		// Absolute가 아닌 상대 Cursor 이동 전략을 사용한 Dirty Run 횟수
		size_t relativeCursorCount = 0;

		// WriteFile이 실제로 기록했다고 반환한 바이트 수
		size_t writtenByteCount = 0;

		// Present() 내부 측정
		double buildOutputTimeMs = 0.0;

		double terminalWriteTimeMs = 0.0;

		double terminalFlushTimeMs = 0.0;
	};

	RendererDebugStats rendererDebugStats;

	// 시간 차이를 밀리초 단위로 변환
	double ToMilliseconds(const std::chrono::steady_clock::duration& duration)
	{
		return std::chrono::duration<double, std::milli>(duration).count();
	}

	// Present()에서 사용할 디버그 카운터 초기화
	void ResetPresentDebugStats()
	{
		rendererDebugStats.dirtyCellCount = 0;
		rendererDebugStats.dirtyRunCount = 0;

		rendererDebugStats.foregroundChangeCount = 0;
		rendererDebugStats.backgroundChangeCount = 0;

		rendererDebugStats.outputByteCount = 0;

		// Cursor 이동 방식 카운터 초기화
		rendererDebugStats.absoluteCursorCount = 0;
		rendererDebugStats.relativeCursorCount = 0;

		// 실제 WriteFile 기록 바이트 초기화
		rendererDebugStats.writtenByteCount = 0;

		// Present 내부 시간 초기화
		rendererDebugStats.buildOutputTimeMs = 0.0;
		rendererDebugStats.terminalWriteTimeMs = 0.0;
		rendererDebugStats.terminalFlushTimeMs = 0.0;
	}
#endif
	// ============================================================

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

	// Text
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

	// Sprite
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

	// Pixel Sprite
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
#if CRAFT_RENDERER_DEBUG
		// Renderer 성능 측정
		DebugRenderer();

#else
		// 현재 사용할 화면 버퍼 초기화
		Clear();

		// 제출된 RenderCommand를 Cell에 기록
		DrawRenderQueue();

		// 완성된 화면 출력
		Present();
#endif
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

	// =============================PRESENT======================================
	void Renderer::Present()
	{
#if CRAFT_RENDERER_DEBUG
		// 이번 프레임 측정값 초기화 ------------------------------------
		ResetPresentDebugStats();

		using Clock = std::chrono::steady_clock;

		// BuildOutput 측정 시작
		const auto buildOutputStart = Clock::now();
#endif
		// 현재 완성된 화면 버퍼
		ScreenBuffer* currentBuffer = GetCurrentBuffer();

		// 이전 프레임 버퍼
		ScreenBuffer* previousBuffer = GetPreviousBuffer();

		// Present에서는 ScreenBuffer의 연속 Cell 배열을 직접 읽음
		// GetCell(Vector2)의 좌표 검사/인덱스 계산 반복을 제거하기 위함
		const std::vector<Cell>& currentCells = currentBuffer->GetCells();

		const std::vector<Cell>& previousCells = previousBuffer->GetCells();

		// 콘솔에 한 번만 출력할 문자열
		std::string output;

		// 어느정도 메모리 확보 - 매번 문자열이 재할당 되는 것을 줄임
		output.reserve(static_cast<size_t>(screenSize.x * screenSize.y * 8));

		// ANSI 색상 캐시
		ColorRGB cachedForeground;
		ColorRGB cachedBackground;

		bool hasForeground = false;
		bool hasBackground = false;

		// ANSI Cursor 위치 캐시
		// 0-based 좌표 사용
		int cachedCursorX = 0;
		int cachedCursorY = 0;
		
		// 아직 output에서 커서 위치를 확정한 적이 있는지
		bool hasCursorPosition = false;

		// 행 단위로 Previous / Current 비교
		for (int y = 0; y < screenSize.y; ++y)
		{
			// 현재 행이 1차원 Cell 배열에서 시작되는 위치
			const int rowStartIndex = y * screenSize.x;

			int x = 0;

			while (x < screenSize.x)
			{
				const int index = rowStartIndex + x;

				const Cell& currentCell = currentCells[index];

				const Cell& previousCell = previousCells[index];

				// 이전 프레임과 같으면 출력 필요 없음
				if (currentCell == previousCell)
				{
					++x;

					continue;
				}

				// 변경된 구간 시작 - Dirty Run
				const int runStartX = x;

#if CRAFT_RENDERER_DEBUG
				// Dirty Run 하나 발견 ------------------------------------------
				++rendererDebugStats.dirtyRunCount;
#endif
				// 변경된 Cell이 연속되는 동안 이동
				while (x < screenSize.x)
				{
					const int compareIndex = rowStartIndex + x;

					const Cell& current = currentCells[compareIndex];

					const Cell& previous = previousCells[compareIndex];

					// 같은 Cell을 만나면 Dirty Run 종료
					if (current == previous)
					{
						break;
					}

#if CRAFT_RENDERER_DEBUG
					// 이전 프레임과 다른 Cell 하나 발견 ------------------------------------------
					++rendererDebugStats.dirtyCellCount;
#endif
					++x;
				}

				const int runEndX = x;

				// 변경된 영역의 시작점으로 커서 이동
				if (!hasCursorPosition)
				{
#if CRAFT_RENDERER_DEBUG
					++rendererDebugStats.absoluteCursorCount;
#endif
					// 첫 Dirty Run은 안전하게 절대 위치 이동
					ANSIEncoder::AppendCursorPosition(output, runStartX + 1, y + 1);
				}
				else if (cachedCursorY == y && cachedCursorX == runStartX)
				{
					// 이미 정확히 다음 Dirty Run 시작 위치에 있음.
					// ANSI 커서 이동 자체가 필요 없음.
				}
				else if (cachedCursorY == y && cachedCursorX < runStartX)
				{
					// 같은 행에서 오른쪽으로 이동하는 경우
					const int distance = runStartX - cachedCursorX;
					const int relativeCost = RelativeCursorByteCount(distance);
					const int absoluteCost = AbsoluteCursorByteCount(runStartX + 1, y + 1);

					// 실제 ANSI 문자열이 더 짧은 쪽 선택
					if (relativeCost < absoluteCost)
					{
#if CRAFT_RENDERER_DEBUG
						++rendererDebugStats.relativeCursorCount;
#endif
						ANSIEncoder::AppendCursorForward(output, distance);
					}
					else
					{
#if CRAFT_RENDERER_DEBUG
						++rendererDebugStats.absoluteCursorCount;
#endif
						ANSIEncoder::AppendCursorPosition(output, runStartX + 1, y + 1);
					}
				}
				// 다른 행으로 이동하는 경우
				else
				{
					// 현재 위치보다 아래 행으로 이동하는 경우
					if (cachedCursorY < y &&hasCursorPosition)
					{
						const int rowDistance = y - cachedCursorY;

						// ANSI 좌표는 1-based
						const int targetColumn = runStartX + 1;

						// 1. Absolute -> ESC[row;columnH
						const int absoluteCost = AbsoluteCursorByteCount(targetColumn, y + 1);

						// 2. Cursor Down + Horizontal Absolute
						// ESC[nB	|	ESC[columnG
						const int downColumnCost = CursorDownByteCount(rowDistance) + CursorColumnByteCount(targetColumn);

						// 3. Cursor Next Line + Forward
						// CursorNextLine은 이동 후 1열에 위치 -> runStartX == 0이면 Forward가 필요 없음
						int nextLineCost = CursorNextLineByteCount(rowDistance);

						if (runStartX > 0)
						{
							nextLineCost += RelativeCursorByteCount(runStartX);
						}

						// 가장 짧은 ANSI 방식 선택
						if (nextLineCost < absoluteCost && nextLineCost < downColumnCost)
						{
#if CRAFT_RENDERER_DEBUG
							++rendererDebugStats.relativeCursorCount;
#endif
							// 아래 행으로 이동하면서 1열로 이동
							ANSIEncoder::AppendCursorNextLine(output, rowDistance);

							// 목표 위치가 첫 열이 아니면 오른쪽으로 추가 이동
							if (runStartX > 0)
							{
								ANSIEncoder::AppendCursorForward(output, runStartX);
							}
						}
						else if (downColumnCost < absoluteCost)
						{
#if CRAFT_RENDERER_DEBUG
							++rendererDebugStats.relativeCursorCount;
#endif
							// 현재 column을 유지한 채 아래로 이동
							ANSIEncoder::AppendCursorDown(output, rowDistance);

							// 목표 열로 절대 이동
							ANSIEncoder::AppendCursorColumn(output, targetColumn);
						}
						else
						{
#if CRAFT_RENDERER_DEBUG
							++rendererDebugStats.absoluteCursorCount;
#endif
							// 기존 Absolute가 가장 짧은 경우
							ANSIEncoder::AppendCursorPosition(output, targetColumn, y + 1);
						}
					}
					else
					{
#if CRAFT_RENDERER_DEBUG
						++rendererDebugStats.absoluteCursorCount;
#endif
						// 위쪽 행으로 돌아가거나 위치 추적이 불가능한 경우에는
						// 기존 Absolute 방식 사용
						ANSIEncoder::AppendCursorPosition(output, runStartX + 1, y + 1);
					}
				}

				// 커서는 이제 Dirty Run 시작 위치에 있음
				cachedCursorX = runStartX;

				cachedCursorY = y;

				hasCursorPosition = true;

				// 변경된 Cell만 출력 - Dirty Run 출력
				for (int drawX = runStartX;	drawX < runEndX; ++drawX)
				{
					const int drawIndex = rowStartIndex + drawX;

					const Cell& cell = currentCells[drawIndex];

					// Color Change 확인
					const bool foregroundChanged = !hasForeground || cell.foreground != cachedForeground;

					const bool backgroundChanged = !hasBackground || cell.background != cachedBackground;

					// Foreground + Background 둘 다 변경
					
					// 전경/배경색이 둘 다 바뀐 경우에만 ANSI 코드 출력
					if (foregroundChanged && backgroundChanged)
					{
#if CRAFT_RENDERER_DEBUG
						// 실제 ANSI 전경/배경색 변경 발생 --------------------------------
						++rendererDebugStats.foregroundChangeCount;
						++rendererDebugStats.backgroundChangeCount;
#endif
						// 두 색상을 하나의 ANSI SGR 코드로 출력
						ANSIEncoder::AppendColors(output, cell.foreground, cell.background);

						cachedForeground = cell.foreground;
						cachedBackground = cell.background;

						hasForeground = true;
						hasBackground = true;
					}
					// 전경색만 변경
					else if (foregroundChanged)
					{
#if CRAFT_RENDERER_DEBUG
						++rendererDebugStats.foregroundChangeCount;
#endif
						ANSIEncoder::AppendForeground(output, cell.foreground);

						cachedForeground = cell.foreground;
						hasForeground = true;
					}
					// 배경색만 변경
					else if (backgroundChanged)
					{
#if CRAFT_RENDERER_DEBUG
						++rendererDebugStats.backgroundChangeCount;
#endif
						ANSIEncoder::AppendBackground(output, cell.background);

						cachedBackground = cell.background;
						hasBackground = true;
					}
					
					// 실제 문자만 출력
					output += cell.character;
				}

				// Dirty Run 문자를 출력했으므로 터미널 커서는 마지막 출력 Cell의 다음 위치에 있음
				cachedCursorX = runEndX;

				cachedCursorY = y;
			}
		}
		
		if (!output.empty())
		{
			// 색상 초기화
			output += ANSIEncoder::Reset();

#if CRAFT_RENDERER_DEBUG
			// 실제 터미널로 전달되는 ANSI 문자열 크기 기록 ------------------------------
			rendererDebugStats.outputByteCount += output.size();
#endif
		}

#if CRAFT_RENDERER_DEBUG
		// BuildOutput 측정 종료
		const auto buildOutputEnd = Clock::now();
		rendererDebugStats.buildOutputTimeMs = ToMilliseconds(buildOutputEnd - buildOutputStart);
#endif
		// 실제 Terminal Output
		WriteTerminal(output);

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
		const Vector2 screenPosition = camera->WorldToScreen(Vector2F(static_cast<float>(command.position.x), static_cast<float>(command.position.y)));

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
		const Vector2 screenPosition = camera->WorldToScreen(Vector2F(static_cast<float>(command.position.x), static_cast<float>(command.position.y)));

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

	// ======================================================================
	void Renderer::DebugRenderer()
	{
#if CRAFT_RENDERER_DEBUG
		using Clock = std::chrono::steady_clock;

		// 프레임 렌더링 전체 시작 시점
		const auto frameStart = Clock::now();

		// Clear 시간 측정
		const auto clearStart = Clock::now();

		// 현재 사용할 화면 버퍼 초기화
		Clear();

		const auto clearEnd = Clock::now();

		// DrawRenderQueue 시간 측정
		const auto drawQueueStart = Clock::now();

		// 제출된 RenderCommand를 Cell에 기록
		DrawRenderQueue();

		const auto drawQueueEnd = Clock::now();

		// Present 시간 측정
		const auto presentStart = Clock::now();

		// 완성된 화면 출력
		Present();

		const auto presentEnd = Clock::now();

		const auto frameEnd = Clock::now();

		// 각 구간 시간 계산
		const double clearMs = ToMilliseconds(clearEnd - clearStart);

		const double drawQueueMs = ToMilliseconds(drawQueueEnd - drawQueueStart);

		const double presentMs = ToMilliseconds(presentEnd - presentStart);

		const double frameMs = ToMilliseconds(frameEnd - frameStart);

		// 현재 렌더링 FPS 계산
		const double renderFPS = frameMs > 0.0 ? 1000.0 / frameMs : 0.0;

		constexpr double DROP_FPS = 40.0;

		if (renderFPS < DROP_FPS)
		{
			std::ostringstream stream;

			stream
				<< "\n"
				<< "========== RENDER DEBUG ==========\n"

				<< "Render FPS : "
				<< renderFPS
				<< "\n"

				<< "Frame      : "
				<< frameMs
				<< " ms\n"

				<< "Clear      : "
				<< clearMs
				<< " ms\n"

				<< "DrawQueue  : "
				<< drawQueueMs
				<< " ms\n"

				<< "Present    : "
				<< presentMs
				<< " ms\n"

				<< " Build     : "
				<< rendererDebugStats.buildOutputTimeMs
				<< " ms\n"

				<< " Write     : "
				<< rendererDebugStats.terminalWriteTimeMs
				<< " ms\n"

				<< " Flush     : "
				<< rendererDebugStats.terminalFlushTimeMs
				<< " ms\n"

				<< "DirtyCells : "
				<< rendererDebugStats.dirtyCellCount
				<< "\n"

				<< "DirtyRuns  : "
				<< rendererDebugStats.dirtyRunCount
				<< "\n"

				<< "Abs Cursor : "
				<< rendererDebugStats.absoluteCursorCount
				<< "\n"

				<< "Rel Cursor : "
				<< rendererDebugStats.relativeCursorCount
				<< "\n"

				<< "FG Changes : "
				<< rendererDebugStats.foregroundChangeCount
				<< "\n"

				<< "BG Changes : "
				<< rendererDebugStats.backgroundChangeCount
				<< "\n"

				<< "OutputBytes: "
				<< rendererDebugStats.outputByteCount
				<< "\n"

				<< "WrittenBytes: "
				<< rendererDebugStats.writtenByteCount
				<< "\n"

				<< "================================\n";


			OutputDebugStringA(
				stream.str().c_str()
			);
		}
#endif
	}

	void Renderer::WriteTerminal(const std::string& output)
	{
		if (output.empty())
		{
			return;
		}

		switch (TERMINAL_WRITE_MODE)
		{
		case TerminalWriteMode::StdCout:
		{
#if CRAFT_RENDERER_DEBUG
			using Clock = std::chrono::steady_clock;
		
			// Write 측정 시작
			const auto terminalWriteStart = Clock::now();
#endif
			std::cout << output;

#if CRAFT_RENDERER_DEBUG
			// Write 측정 종료
			const auto terminalWriteEnd = Clock::now();
			rendererDebugStats.terminalWriteTimeMs = ToMilliseconds(terminalWriteEnd - terminalWriteStart);

			// Flush 측정 시작
			const auto terminalFlushStart = Clock::now();

#endif
			std::cout.flush();

#if CRAFT_RENDERER_DEBUG
			// Flush 측정 종료
			const auto terminalFlushEnd = Clock::now();

			rendererDebugStats.terminalFlushTimeMs = ToMilliseconds(terminalFlushEnd - terminalFlushStart);
#endif
			break;
		}

		case TerminalWriteMode::WriteFile:
		{
			static HANDLE outputHandle = GetStdHandle(STD_OUTPUT_HANDLE);

			if (outputHandle == nullptr || outputHandle == INVALID_HANDLE_VALUE)
			{
				return;
			}

			DWORD writtenBytes = 0;

#if CRAFT_RENDERER_DEBUG
			using Clock = std::chrono::steady_clock;

			// WriteFile 측정 시작
			const auto terminalWriteStart = Clock::now();
#endif
			WriteFile(outputHandle, output.data(), static_cast<DWORD>(output.size()), &writtenBytes, nullptr);

#if CRAFT_RENDERER_DEBUG
			// 실제 WriteFile 기록 바이트 수 저장
			rendererDebugStats.writtenByteCount = static_cast<size_t>(writtenBytes);

			// WriteFile 측정 종료
			const auto terminalWriteEnd = Clock::now();

			rendererDebugStats.terminalWriteTimeMs = ToMilliseconds(terminalWriteEnd - terminalWriteStart);

			// WriteFile에는 std::cout.flush()에 해당하는 ostream Flush 단계가 없음
			rendererDebugStats.terminalFlushTimeMs = 0.0;
#endif
			break;
		}
		}
	}
}