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
	// 플레이어 부위 연결
	this->head = head;
	this->body = body;
	this->legs = legs;

	this->leftHand = leftHand;
	this->rightHand = rightHand;

	// sprite 데이터 생성
	GenerateVisualData();

	isInitialized = true;
}

void PlayerVisual::SetFacingRight(bool facingRight)
{
	if (!isInitialized)
	{
		return;
	}

	isHeadFacingRight = facingRight;
	isOtherFacingRight = facingRight;
}

void PlayerVisual::SetHeadFacingRight(bool facingRight)
{
	if (!isInitialized)
	{
		return;
	}

	// 머리 방향만 갱신
	isHeadFacingRight = facingRight;
}

void PlayerVisual::SetDiveDirection(const Craft::Vector2F& direction)
{
	diveDirection = direction;
}

int PlayerVisual::GetAnimationFrameCount(PlayerAnimationState state) const
{
	// 각 상태에서 부위별 애니메이션 프레임이 가장 긴 애니메이션을 기준으로 함
	switch (state)
	{
	case PlayerAnimationState::Idle:
		return static_cast<int>(idleHead.frames.size());

	case PlayerAnimationState::Walk:
		return static_cast<int>(walkLegs.frames.size());

	case PlayerAnimationState::Dive:
		return static_cast<int>(diveBody.frames.size());

	case PlayerAnimationState::Fire:
		return 0;
	default:
		return 0;
		
	}
}

float PlayerVisual::GetAnimationFrameDuration(PlayerAnimationState state) const
{
	// 각 상태에서 부위별 애니메이션 프레임이 가장 긴 애니메이션을 기준으로 함
	switch (state)
	{
	case PlayerAnimationState::Idle:
		return idleHead.frameDuration;

	case PlayerAnimationState::Walk:
		return walkLegs.frameDuration;

	case PlayerAnimationState::Dive:
		return diveBody.frameDuration;

	case PlayerAnimationState::Fire:
		return 0.0f;
	default:
		return 0.0f;
	}
}

bool PlayerVisual::IsAnimationLooping(PlayerAnimationState state) const
{
	// 각 상태에서 부위별 애니메이션 프레임이 가장 긴 애니메이션을 기준으로 함
	switch (state)
	{
	case PlayerAnimationState::Idle:
		return idleHead.loop;

	case PlayerAnimationState::Walk:
		return walkLegs.loop;

	case PlayerAnimationState::Dive:
		return diveBody.loop;

	case PlayerAnimationState::Fire:
		return false;
	default:
		return false;
	}
}

void PlayerVisual::ApplyAnimationFrame(PlayerAnimationState state, int frameIndex)
{
	if (!isInitialized)
	{
		return;
	}

	const CharacterPartAnimation* headAnimation = nullptr;
	const CharacterPartAnimation* bodyAnimation = nullptr;
	const CharacterPartAnimation* legsAnimation = nullptr;
	const CharacterPartAnimation* leftHandAnimation = nullptr;
	const CharacterPartAnimation* rightHandAnimation = nullptr;

	switch (state)
	{
	case PlayerAnimationState::Idle:
		headAnimation = &idleHead;
		bodyAnimation = &idleBody;
		legsAnimation = &idleLegs;
		leftHandAnimation = &idleLeftHand;
		rightHandAnimation = &idleRightHand;

		currentVisualWidth = playerWidth;
		break;

	case PlayerAnimationState::Walk:
		headAnimation = &walkHead;
		bodyAnimation = &walkBody;
		legsAnimation = &walkLegs;
		leftHandAnimation = &walkLeftHand;
		rightHandAnimation = &walkRightHand;

		currentVisualWidth = playerWidth;
		break;

	case PlayerAnimationState::Dive:
		headAnimation = &diveHead;
		bodyAnimation = &diveBody;
		legsAnimation = &diveLegs;
		leftHandAnimation = &diveLeftHand;
		rightHandAnimation = &diveRightHand;

		currentVisualWidth = diveWidth;
		break;

	case PlayerAnimationState::Fire:
		break;
	default:
		return;
	}

	// 각 부위에 애니메이션 프레임 적용
	ApplyPartFrame(head, headAnimation, frameIndex);
	ApplyPartFrame(body, bodyAnimation, frameIndex);
	ApplyPartFrame(legs, legsAnimation, frameIndex);
	ApplyPartFrame(leftHand, leftHandAnimation, frameIndex);
	ApplyPartFrame(rightHand, rightHandAnimation, frameIndex);

	if (state == PlayerAnimationState::Dive)
	{
		ApplyDiveDirection();
	}
	else
	{
		// 애니메이션 프레임 적용 뒤 방향 계산
		ApplyFacing();
	}
}

void PlayerVisual::GenerateVisualData()
{
	// 기본 정지 상태
	GenerateIdleAnimation();

	// 걷기 상태
	GenerateWalkAnimation();

	// 다이브 상태
	GenerateDiveAnimation();
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
			6
		});

	idleRightHand.frames.push_back(
		CharacterPartFrame{
			handSprite,
			Craft::Vector2(7, 8),
			4
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
		".OL...BBB",
		".BBB....."
	};

	const char* walkLegPixels2[2] =
	{
		".BBB..OL.",
		"......BBB"
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

void PlayerVisual::GenerateDiveAnimation()
{
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
	// 양쪽 외곽선은 LeftHand / RightHand 쪽으로 분리했다.
	// ========================================================
	const char* bodyPixels[5] =
	{
		"OOOOOOOOO",
		"AAAaaaAAY",
		"AAAaaAAAY",
		"AAAaYYYAY",
		"OOOOOOOOO"
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
	// ========================================================
	const char* legsPixels[3] =
	{
		"BBBO",
		"BBLL",
		".BB."
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
			5,
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
			4,
			3,
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

	diveHead.frames.push_back(
		CharacterPartFrame{
			headSprite,
			Craft::Vector2(11, 1),
			2
		});

	diveBody.frames.push_back(
		CharacterPartFrame{
			bodySprite,
			Craft::Vector2(4, 3),
			1
		});

	diveLegs.frames.push_back(
		CharacterPartFrame{
			legsSprite,
			Craft::Vector2(0, 3),
			0
		});

	diveLeftHand.frames.push_back(
		CharacterPartFrame{
			handSprite,
			Craft::Vector2(8, 4),
			6
		});

	diveRightHand.frames.push_back(
		CharacterPartFrame{
			handSprite,
			Craft::Vector2(11, 5),
			4
		});

	diveHead.frameDuration = 0.35f;
	diveBody.frameDuration = 0.35f;
	diveLegs.frameDuration = 0.35f;
	diveLeftHand.frameDuration = 0.35f;
	diveRightHand.frameDuration = 0.35f;

	diveHead.loop = false;
	diveBody.loop = false;
	diveLegs.loop = false;
	diveLeftHand.loop = false;
	diveRightHand.loop = false;
}

void PlayerVisual::GenerateFireAnimation()
{}

void PlayerVisual::ApplyPartFrame(CharacterPart* part, const CharacterPartAnimation* animation, int frameIndex)
{
	if (!part || !animation || animation->frames.empty())
	{
		return;
	}

	// 부위마다 프레임 개수가 달라도 사용할 수 있도록 처리
	const int partFrameIndex = frameIndex % static_cast<int>(animation->frames.size());

	const CharacterPartFrame& frame = animation->frames[partFrameIndex];

	part->SetSprite(frame.sprite);
	part->SetLocalPosition(frame.localPosition);
	part->SetSortingOffset(frame.sortingOffset);
}

void PlayerVisual::ApplyFacing()
{
	// Head / Body / Legs Sprite 좌우 반전
	if (!isHeadFacingRight && head)
	{
		head->SetSprite(FlipHorizontal(head->GetSprite()));

		Craft::Vector2 position = head->GetLocalPosition();

		position.x = currentVisualWidth - head->GetSprite().GetWidth() - position.x;

		head->SetLocalPosition(position);
	}

	if (!isOtherFacingRight)
	{
		if (body)
		{
			body->SetSprite(FlipHorizontal(body->GetSprite()));

			Craft::Vector2 position = body->GetLocalPosition();

			position.x = currentVisualWidth - body->GetSprite().GetWidth() - position.x;

			body->SetLocalPosition(position);
		}

		if (legs)
		{
			legs->SetSprite(FlipHorizontal(legs->GetSprite()));

			Craft::Vector2 position = legs->GetLocalPosition();

			position.x = currentVisualWidth - legs->GetSprite().GetWidth() - position.x;

			legs->SetLocalPosition(position);
		}

		// 손 위치 좌우 반전
		if (leftHand)
		{
			leftHand->SetSprite(FlipHorizontal(leftHand->GetSprite()));

			Craft::Vector2 position = leftHand->GetLocalPosition();

			position.x = currentVisualWidth - leftHand->GetSprite().GetWidth() - position.x;

			leftHand->SetLocalPosition(position);
		}

		if (rightHand)
		{
			rightHand->SetSprite(FlipHorizontal(rightHand->GetSprite()));

			Craft::Vector2 position = rightHand->GetLocalPosition();

			position.x = currentVisualWidth - rightHand->GetSprite().GetWidth() - position.x;

			rightHand->SetLocalPosition(position);
		}
	}
}

void PlayerVisual::ApplyDiveDirection()
{
	// 오른쪽
	if (diveDirection.x >= 0.0f)
	{
		head->SetLocalPosition(Craft::Vector2(11, 1));
		body->SetLocalPosition(Craft::Vector2(4, 3));
		legs->SetLocalPosition(Craft::Vector2(0, 3));
		leftHand->SetLocalPosition(Craft::Vector2(8, 4));
		rightHand->SetLocalPosition(Craft::Vector2(11, 5));
	}
	// 왼쪽
	else
	{
		head->SetLocalPosition(Craft::Vector2(0, 1));
		body->SetLocalPosition(Craft::Vector2(7, 3));
		legs->SetLocalPosition(Craft::Vector2(16, 3));
		leftHand->SetLocalPosition(Craft::Vector2(7, 4));
		rightHand->SetLocalPosition(Craft::Vector2(5, 5));
	}

	// 머리 시선 처리
	ApplyDiveHeadDirection();
}

void PlayerVisual::ApplyDiveHeadDirection()
{
	if (!head)
	{
		return;
	}

	if (!isHeadFacingRight)
	{
		// 마우스가 왼쪽이면 왼쪽을 바라보게 함
		head->SetSprite(FlipHorizontal(head->GetSprite()));
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
