#include "PlayerVisual.h"
#include <Actor/Character/CharacterPart.h>

using namespace Craft;
namespace
{
	Craft::Cell MakePixel(const Craft::ColorRGB& color)
	{
		Craft::Cell cell;

		cell.character = ' ';
		cell.foreground = color;
		cell.background = color;

		return cell;
	};

	Craft::PixelSprite MakePartSprite(
		int width,
		int height,
		const char* const* pixels,
		const Craft::Cell& outlinePixel,
		const Craft::Cell& helmetPixel,
		const Craft::Cell& helmetLightPixel,
		const Craft::Cell& visorPixel,
		const Craft::Cell& armorPixel,
		const Craft::Cell& armorLightPixel,
		const Craft::Cell& yellowPixel,
		const Craft::Cell& legPixel,
		const Craft::Cell& bootPixel,
		const Craft::Cell& handPixel)
	{
		Craft::PixelSprite sprite(width, height);

		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				switch (pixels[y][x])
				{
				case 'O':
					sprite.SetCell(x, y, outlinePixel);
					break;

				case 'H':
					sprite.SetCell(x, y, helmetPixel);
					break;

				case 'h':
					sprite.SetCell(x, y, helmetLightPixel);
					break;

				case 'V':
					sprite.SetCell(x, y, visorPixel);
					break;

				case 'A':
					sprite.SetCell(x, y, armorPixel);
					break;

				case 'a':
					sprite.SetCell(x, y, armorLightPixel);
					break;

				case 'Y':
					sprite.SetCell(x, y, yellowPixel);
					break;

				case 'L':
					sprite.SetCell(x, y, legPixel);
					break;

				case 'B':
					sprite.SetCell(x, y, bootPixel);
					break;

				case 'P':
					sprite.SetCell(x, y, handPixel);
					break;

				case '.':
				default:
					// 투명 처리
					break;
				}
			}
		}

		return sprite;
	}
}

void PlayerVisual::Initialize(
	CharacterPart* head,
	CharacterPart* body,
	CharacterPart* legs,
	CharacterPart* leftHand,
	CharacterPart* rightHand)
{
	this->head = head;
	this->body = body;
	this->legs = legs;

	this->leftHand = leftHand;
	this->rightHand = rightHand;

	GenerateVisualData();

	currentState = PlayerAnimationState::Idle;

	currentFrame = 0;

	frameTimer = 0.0f;

	isFacingRight = true;

	isInitialized = true;

	ApplyCurrentFrame();
}

void PlayerVisual::Tick(float deltaTime)
{
	if (!isInitialized)
	{
		return;
	}

	UpdateAnimation(deltaTime);
}

void PlayerVisual::SetFacingRight(bool facingRight)
{
	if (!isInitialized)
	{
		return;
	}

	if (isFacingRight == facingRight)
	{
		return;
	}

	isFacingRight = facingRight;

	// 원본 프레임을 다시 적용한 뒤 현재 방향에 맞게 재계산
	ApplyCurrentFrame();
}

void PlayerVisual::SetAnimationState(PlayerAnimationState state)
{
	if (currentState == state)
	{
		return;
	}

	currentState = state;

	currentFrame = 0;

	frameTimer = 0.0f;

	ApplyCurrentFrame();
}

void PlayerVisual::GenerateVisualData()
{
	// 기본 정지 상태
	GenerateIdleAnimation();

	// 걷기 상태
	GenerateWalkAnimation();
}

void PlayerVisual::GenerateIdleAnimation()
{
	// ----------------------------------------------------
	// 색상
	// ----------------------------------------------------

	const Craft::ColorRGB outlineColor(20, 22, 20);     // 외곽선
	const Craft::ColorRGB helmetColor(45, 49, 45);      // 헬멧
	const Craft::ColorRGB helmetLightColor(68, 72, 65); // 헬멧 밝은 면
	const Craft::ColorRGB visorColor(40, 85, 95);       // 어두운 바이저

	const Craft::ColorRGB armorColor(58, 62, 57);       // 방탄복
	const Craft::ColorRGB armorLightColor(82, 86, 78);  // 방탄복 밝은 면

	const Craft::ColorRGB yellowColor(220, 185, 45);    // 노란 포인트

	const Craft::ColorRGB handColor(120, 125, 120);		// 손

	const Craft::ColorRGB legColor(45, 48, 44);         // 다리
	const Craft::ColorRGB bootColor(26, 28, 26);        // 부츠

	// Cell 변환
	const Craft::Cell outlinePixel = MakePixel(outlineColor);

	const Craft::Cell helmetPixel = MakePixel(helmetColor);

	const Craft::Cell helmetLightPixel = MakePixel(helmetLightColor);

	const Craft::Cell visorPixel = MakePixel(visorColor);

	const Craft::Cell armorPixel = MakePixel(armorColor);

	const Craft::Cell armorLightPixel = MakePixel(armorLightColor);

	const Craft::Cell yellowPixel = MakePixel(yellowColor);

	const Craft::Cell handPixel = MakePixel(handColor);

	const Craft::Cell legPixel = MakePixel(legColor);

	const Craft::Cell bootPixel = MakePixel(bootColor);

	// ========================================================
	// Head
	//
	// 크기: 9 x 7
	// Character 기준 위치: (0, 0)
	// ========================================================
	const char* headPixels[7] =
	{
		"..OOOOOO.",
		".OHHYHHYO",
		"OHHhhhhhO",
		"OHHVVVVVO",
		"OHHHVVVVO",
		"OHHHHYYHO",
		"OHHHHHYHO"
	};

	// ========================================================
	// Body
	//
	// 기존 Player Sprite의 7~12행.
	//
	// 양쪽 외곽선은 LeftHand / RightHand 쪽으로 분리했다.
	// ========================================================
	const char* bodyPixels[6] =
	{
		".OYYYYAO.",
		".OAAAAAO.",
		".OAaaaYO.",
		".OAAAAAO.",
		".OAAAAAO.",
		".OYYYYYO."
	};

	// ========================================================
	// LeftHand / RightHand
	//
	// 2 x 2 크기의 밝은 회색 손 Sprite
	// ========================================================
	const char* handPixels[2] =
	{
		"PP",
		"PP"
	};

	// ========================================================
	// Legs
	//
	// 기존 Player Sprite의 13~14행
	// ========================================================
	const char* legsPixels[2] =
	{
		".OL..OL..",
		".BBB.BBB."
	};

	// 머리
	const Craft::PixelSprite headSprite =
		MakePartSprite(
			9,
			7,
			headPixels,
			outlinePixel,
			helmetPixel,
			helmetLightPixel,
			visorPixel,
			armorPixel,
			armorLightPixel,
			yellowPixel,
			legPixel,
			bootPixel,
			handPixel);


	// 몸통
	const Craft::PixelSprite bodySprite =
		MakePartSprite(
			9,
			6,
			bodyPixels,
			outlinePixel,
			helmetPixel,
			helmetLightPixel,
			visorPixel,
			armorPixel,
			armorLightPixel,
			yellowPixel,
			legPixel,
			bootPixel,
			handPixel);

	// 다리
	const Craft::PixelSprite legsSprite =
		MakePartSprite(
			9,
			2,
			legsPixels,
			outlinePixel,
			helmetPixel,
			helmetLightPixel,
			visorPixel,
			armorPixel,
			armorLightPixel,
			yellowPixel,
			legPixel,
			bootPixel,
			handPixel);

	// 손
	const Craft::PixelSprite handSprite =
		MakePartSprite(
			2,
			2,
			handPixels,
			outlinePixel,
			helmetPixel,
			helmetLightPixel,
			visorPixel,
			armorPixel,
			armorLightPixel,
			yellowPixel,
			legPixel,
			bootPixel,
			handPixel);

	idleHead.frames.push_back(
		CharacterPartFrame{
			headSprite,
			Craft::Vector2(0, 0),
			2
		});

	idleHead.frames.push_back(
		CharacterPartFrame{
			headSprite,
			Craft::Vector2(0, 1),
			2
		});

	idleBody.frames.push_back(
		CharacterPartFrame{
			bodySprite,
			Craft::Vector2(0, 7),
			1
		});

	idleLegs.frames.push_back(
		CharacterPartFrame{
			legsSprite,
			Craft::Vector2(0, 13),
			0
		});

	idleLeftHand.frames.push_back(
		CharacterPartFrame{
			handSprite,
			Craft::Vector2(2, 9),
			3
		});

	idleRightHand.frames.push_back(
		CharacterPartFrame{
			handSprite,
			Craft::Vector2(7, 8),
			3
		});

	idleHead.frameDuration = 0.35f;

	idleHead.loop = true;
}

void PlayerVisual::GenerateWalkAnimation()
{
	// Idle 데이터가 정상 생성되어 있지 않으면 Walk 데이터도 만들 수 없음
	if (idleHead.frames.empty() ||
		idleBody.frames.empty() ||
		idleLegs.frames.empty() ||
		idleLeftHand.frames.empty() ||
		idleRightHand.frames.empty())
	{
		return;
	}

	// 기존 Idle Sprite 재사용
	const Craft::PixelSprite& headSprite = idleHead.frames[0].sprite;

	const Craft::PixelSprite& bodySprite = idleBody.frames[0].sprite;

	const Craft::PixelSprite& idleLegsSprite = idleLegs.frames[0].sprite;

	const Craft::PixelSprite& handSprite = idleLeftHand.frames[0].sprite;

	// 다리 Walk Sprite 제작에 필요한 색상
	const Craft::ColorRGB outlineColor(20, 22, 20);

	const Craft::ColorRGB legColor(45, 48, 44);

	const Craft::ColorRGB bootColor(26, 28, 26);

	const Craft::Cell outlinePixel = MakePixel(outlineColor);

	const Craft::Cell legPixel = MakePixel(legColor);

	const Craft::Cell bootPixel = MakePixel(bootColor);

	const Craft::Cell emptyPixel;

	// Walk Legs Frame
	const char* walkLegPixels1[2] =
	{
		".OL...OL.",
		".BBB..BBB"
	};

	const char* walkLegPixels2[2] =
	{
		"..OL.OL..",
		"..BBB.BBB"
	};

	const Craft::PixelSprite walkLegSprite1 =
		MakePartSprite(
			9,
			2,
			walkLegPixels1,
			outlinePixel,
			emptyPixel,     // helmet
			emptyPixel,     // helmetLight
			emptyPixel,     // visor
			emptyPixel,     // armor
			emptyPixel,     // armorLight
			emptyPixel,     // yellow
			legPixel,
			bootPixel,
			emptyPixel);    // hand

	const Craft::PixelSprite walkLegSprite2 =
		MakePartSprite(
			9,
			2,
			walkLegPixels2,
			outlinePixel,
			emptyPixel,
			emptyPixel,
			emptyPixel,
			emptyPixel,
			emptyPixel,
			emptyPixel,
			legPixel,
			bootPixel,
			emptyPixel);

	// 머리
	for (int i = 0; i < 3; ++i)
	{
		walkHead.frames.push_back(CharacterPartFrame{headSprite, Craft::Vector2(0, 0), 2});
	}

	// 몸통
	for (int i = 0; i < 3; ++i)
	{
		walkBody.frames.push_back(CharacterPartFrame{bodySprite, Craft::Vector2(0, 7), 1});
	}

	// 다리
	// Frame 0
	walkLegs.frames.push_back(CharacterPartFrame{idleLegsSprite, Craft::Vector2(0, 13), 0});

	// Frame 1
	walkLegs.frames.push_back(CharacterPartFrame{walkLegSprite1, Craft::Vector2(0, 13), 0});

	// Frame 2
	walkLegs.frames.push_back(CharacterPartFrame{walkLegSprite2, Craft::Vector2(0, 13), 0});

	// LeftHand
	walkLeftHand.frames.push_back(CharacterPartFrame{handSprite, Craft::Vector2(2, 9), 3});

	walkLeftHand.frames.push_back(CharacterPartFrame{handSprite, Craft::Vector2(2, 8), 3});

	walkLeftHand.frames.push_back(CharacterPartFrame{handSprite, Craft::Vector2(2, 9), 3});

	// RightHand
	walkRightHand.frames.push_back(CharacterPartFrame{handSprite, Craft::Vector2(7, 8), 3});

	walkRightHand.frames.push_back(CharacterPartFrame{handSprite, Craft::Vector2(7, 9), 3});

	walkRightHand.frames.push_back(CharacterPartFrame{handSprite, Craft::Vector2(7, 8), 3});

	// Walk Animation 속도
	walkHead.frameDuration = 0.12f;
	walkBody.frameDuration = 0.12f;
	walkLegs.frameDuration = 0.12f;

	walkLeftHand.frameDuration = 0.12f;
	walkRightHand.frameDuration = 0.12f;


	// 걷기는 반복 애니메이션
	walkHead.loop = true;
	walkBody.loop = true;
	walkLegs.loop = true;
	walkLeftHand.loop = true;
	walkRightHand.loop = true;
}

void PlayerVisual::GenerateAimAnimation()
{}

void PlayerVisual::GenerateFireAnimation()
{}

void PlayerVisual::UpdateAnimation(float deltaTime)
{
	CharacterPartAnimation* baseAnimation = nullptr;

	switch (currentState)
	{
	case PlayerAnimationState::Idle:

		baseAnimation = &idleHead;
		break;


	case PlayerAnimationState::Walk:

		baseAnimation = &walkLegs;
		break;

	case PlayerAnimationState::Aim:
	case PlayerAnimationState::Fire:
	default:

		return;
	}

	// 예외 처리
	if (!baseAnimation || baseAnimation->frames.empty())
	{
		return;
	}

	// 누적 시간 증가
	frameTimer += deltaTime;

	// 아직 다음 프레임 시간이 아니라면 유지
	if (frameTimer < baseAnimation->frameDuration)
	{
		return;
	}

	// 프레임 시간이 초과된 만큼 제거
	// frameTimer = 0 으로 바로 만드는 것보다 남은 시간을 유지하는 편이 프레임 드랍 상황에서
	// 애니메이션 속도가 덜 흔들림
	frameTimer -= baseAnimation->frameDuration;
	++currentFrame;

	// 마지막 프레임을 넘어가면 다시 첫 프레임으로 순환
	if (currentFrame >= static_cast<int>(baseAnimation->frames.size()))
	{
		currentFrame = 0;
	}

	// 새 Frame 적용
	ApplyCurrentFrame();
}

void PlayerVisual::ApplyCurrentFrame()
{
	if (!isInitialized)
	{
		return;
	}

	// 현재 상태에서 사용할 Animation 선택
	CharacterPartAnimation* headAnimation = nullptr;
	CharacterPartAnimation* bodyAnimation = nullptr;
	CharacterPartAnimation* legsAnimation = nullptr;

	CharacterPartAnimation* leftHandAnimation = nullptr;
	CharacterPartAnimation* rightHandAnimation = nullptr;

	switch (currentState)
	{
	case PlayerAnimationState::Idle:
		headAnimation = &idleHead;

		bodyAnimation = &idleBody;

		legsAnimation = &idleLegs;

		leftHandAnimation = &idleLeftHand;

		rightHandAnimation = &idleRightHand;

		break;

	case PlayerAnimationState::Walk:
		headAnimation =	&walkHead;

		bodyAnimation = &walkBody;

		legsAnimation = &walkLegs;

		leftHandAnimation = &walkLeftHand;

		rightHandAnimation = &walkRightHand;

		break;

	case PlayerAnimationState::Aim:
	case PlayerAnimationState::Fire:
	default:
		return;
	}

	// 머리
	if (head && headAnimation && !headAnimation->frames.empty())
	{
		// 부위마다 Frame 수가 다를 수 있기 때문에 현재 Frame을 해당 Animation 크기로 순환
		const int frameIndex = currentFrame % static_cast<int>(headAnimation->frames.size());
		const CharacterPartFrame& frame = headAnimation->frames[frameIndex];

		head->SetSprite(frame.sprite);
		head->SetLocalPosition(frame.localPosition);
		head->SetSortingOffset(frame.sortingOffset);
	}
	
	// 몸통
	if (body && bodyAnimation && !bodyAnimation->frames.empty())
	{
		const int frameIndex = currentFrame % static_cast<int>(bodyAnimation->frames.size());
		const CharacterPartFrame& frame = bodyAnimation->frames[frameIndex];

		body->SetSprite(frame.sprite);
		body->SetLocalPosition(frame.localPosition);
		body->SetSortingOffset(frame.sortingOffset);
	}

	// 다리	
	if (legs && legsAnimation && !legsAnimation->frames.empty())
	{
		const int frameIndex = currentFrame % static_cast<int>(legsAnimation->frames.size());
		const CharacterPartFrame& frame = legsAnimation->frames[frameIndex];

		legs->SetSprite(frame.sprite);
		legs->SetLocalPosition(frame.localPosition);
		legs->SetSortingOffset(frame.sortingOffset);
	}

	// 왼손	
	if (leftHand && leftHandAnimation && !leftHandAnimation->frames.empty())
	{
		const int frameIndex = currentFrame % static_cast<int>(leftHandAnimation->frames.size());
		const CharacterPartFrame& frame = leftHandAnimation->frames[frameIndex];

		leftHand->SetSprite(frame.sprite);
		leftHand->SetLocalPosition(frame.localPosition);
		leftHand->SetSortingOffset(frame.sortingOffset);
	}

	// 오른손	
	if (rightHand && rightHandAnimation && !rightHandAnimation->frames.empty())
	{
		const int frameIndex = currentFrame % static_cast<int>(rightHandAnimation->frames.size());
		const CharacterPartFrame& frame = rightHandAnimation->frames[frameIndex];

		rightHand->SetSprite(frame.sprite);
		rightHand->SetLocalPosition(frame.localPosition);
		rightHand->SetSortingOffset(frame.sortingOffset);
	}

	// 모든 Frame은 오른쪽 방향을 원본으로 저장
	// Frame 적용이 끝난 뒤 현재 Player 방향에 따라 좌우 반전
	ApplyFacing();
}

void PlayerVisual::ApplyFacing()
{
	// 오른쪽이면 원본 데이터를 그대로 사용
	if (isFacingRight)
	{
		return;
	}

	// Head / Body / Legs Sprite 좌우 반전
	if (head)
	{
		head->SetSprite(FlipHorizontal(head->GetSprite()));
	}

	if (body)
	{
		body->SetSprite(FlipHorizontal(body->GetSprite()));
	}

	if (legs)
	{
		legs->SetSprite(FlipHorizontal(legs->GetSprite()));
	}

	// 손 위치 좌우 반전
	if (leftHand)
	{
		Craft::Vector2 position = leftHand->GetLocalPosition();

		position.x = 9 - leftHand->GetSprite().GetWidth() - position.x;

		leftHand->SetLocalPosition(position);

		leftHand->SetSortingOffset(3);
	}

	if (rightHand)
	{
		Craft::Vector2 position = rightHand->GetLocalPosition();

		position.x = 9 - rightHand->GetSprite().GetWidth() - position.x;

		rightHand->SetLocalPosition(position);

		rightHand->SetSortingOffset(3);
	}
}

Craft::PixelSprite PlayerVisual::FlipHorizontal(const Craft::PixelSprite & sprite) const
{
	const int width = sprite.GetWidth();
	const int height = sprite.GetHeight();

	Craft::PixelSprite flippedSprite(width, height);

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			const int sourceX = width - 1 - x;

			flippedSprite.SetCell(x, y, sprite.GetCell(sourceX, y));
		}
	}

	return flippedSprite;
}
