#pragma once

#include <Core/Core.h>
#include <Core/CraftObject.h>
#include <Math/Vector2.h>
#include <Math/Vector2F.h>
#include <Math/ColorRGB.h>
#include <Render/Sprite/Sprite.h>
#include <Render/Sprite/PixelSprite.h>
#include <memory>		// std::weak_ptr 사용을 위해

namespace Craft
{
	// 전방 선언
	class Level;

	// Actor의 RenderType
	enum class ActorRenderType
	{
		Text,
		PixelSprite
	};

	// 가상 공간에 배치될 모든 액터의 기본 클래스
	class CRAFT_API Actor : public CraftObject, public std::enable_shared_from_this<Actor>
	{
		// 매크로 지정할 때 끝에 세미콜론 넣지 않도록 주의
		TYPE_DECLARATIONS(Actor, CraftObject)

	public:
		// 기본 Actor
		Actor(const Vector2F& position = Vector2F::Zero);

		// 문자열 Sprite 형식
		Actor(
			const Sprite& sprite,
			const Vector2F& position = Vector2F::Zero,
			const ColorRGB& foregroundColor = ColorRGB(255, 255, 255),
			const ColorRGB& backgroundColor = ColorRGB(0, 0, 0)
		);

		// PixelSprite 형식
		Actor(
			const PixelSprite& pixelSprite,
			const Vector2F& position = Vector2F::Zero
		);

		virtual ~Actor();

		// 게임 플레이 이벤트 함수
		virtual void BeginPlay();
		virtual void Tick(float deltaTime);
		virtual void Draw();

		// 충돌 이벤트 함수
		virtual void OnCollision(const std::shared_ptr<Actor>& other);

		// 액터 제거 함수
		void Destroy();

		// 게임(엔진) 종료 함수
		void QuitGame();

		// Getter/Setter
		inline bool HasBeganPlay() const { return hasBeganPlay; }
		inline bool IsActive() const { return isActive && !hasExpired; }
		inline bool HasExpired() const { return hasExpired; }

		inline std::shared_ptr<Level> GetOwner() const { return owner.lock(); }
		inline void SetOwner(std::weak_ptr<Level> newOwner) { owner = newOwner; }

		inline Vector2F GetPosition() const { return position; }
		void SetPosition(const Vector2F& newPosition);

		// 이전 위치 반환 함수
		inline Vector2F GetPreviousPosition() const { return previousPosition; }

		// 프레임 종료 후 이전 프레임 위치 저장 함수
		inline void SavePreviousState() { previousPosition = position; }

		// 현재 Actor 너비
		int GetWidth() const;

		// 현재 Actor 높이
		int GetHeight() const;

		// 문자열 Sprite변경
		void ChangeSprite(const Sprite& newSprite);

		// PixelSprite 변경
		void ChangePixelSprite(const PixelSprite& newPixelSprite);

	protected:
		// BeginPlay 이벤트 처리 여부 플래그
		bool hasBeganPlay = false;

		// 액터 활성화 여부 플래그
		bool isActive = true;

		// 삭제 요청 여부 플래그
		bool hasExpired = false;

		// 오너십 - 이 액터를 소유하는 레벨 객체
		// weak_ptr -> 약참조
		// -> 실제 사용을 위해서는 해당 위치가 유효한지 확인해야함
		std::weak_ptr<Level> owner;

		// 현재 랜더 타입
		ActorRenderType renderType = ActorRenderType::Text;

		// 일반 문자열 Sprite
		Sprite sprite;

		// RGB PixelSprite
		PixelSprite pixelSprite;

		// 문자열 전경색
		ColorRGB foregroundColor = ColorRGB(255, 255, 255);

		// 문자열 배경색
		ColorRGB backgroundColor = ColorRGB(0, 0, 0);

		// 렌더링 순서
		int sortingOrder = 0;

		// 위치
		Vector2F position;

		// 이전 프레임 위치
		Vector2F previousPosition;
	};
}