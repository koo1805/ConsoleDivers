#include "Actor.h"
#include <Engine/Engine.h>
#include <Render/Renderer.h>

namespace Craft
{
	Actor::Actor(const Vector2& position)
		: position(position)
	{ }

	Actor::Actor(
		const Sprite& sprite,
		const Vector2& position,
		const ColorRGB& foregroundColor,
		const ColorRGB& backgroundColor)
		:
		renderType(ActorRenderType::Text),
		sprite(sprite),
		position(position),
		foregroundColor(foregroundColor),
		backgroundColor(backgroundColor)
	{ }

	Actor::Actor(const PixelSprite& pixelSprite, const Vector2& position)
		: renderType(ActorRenderType::PixelSprite),
		pixelSprite(pixelSprite),
		position(position)
	{ }

	Actor::~Actor()
	{ }

	void Actor::BeginPlay()
	{
		// 이벤트 처리했다고 설정
		hasBeganPlay = true;
	}

	void Actor::Tick(float deltaTime)
	{}

	void Actor::Draw()
	{
		// 비활성 상태이면 종료
		if (!IsActive())
		{
			return;
		}

		// 렌더러에 필요한 데이터 제출
		switch (renderType)
		{
		case Craft::ActorRenderType::Text:
			// Sprite의 각 줄의 문자열을 Renderer에 전달
			for (int y = 0; y < sprite.GetHeight(); ++y)
			{
				const std::string& line = sprite.GetLines()[y];

				Renderer::Get().Submit(
					line,
					Vector2(position.x, position.y + y),
					foregroundColor,
					backgroundColor,
					sortingOrder
				);
			}
			break;

		case Craft::ActorRenderType::PixelSprite:
			Renderer::Get().Submit(pixelSprite, position, sortingOrder);
			break;
		}
	}

	void Actor::OnCollision(const std::shared_ptr<Actor>& other)
	{
	}

	void Actor::Destroy()
	{
		// 삭제 예약 설정
		hasExpired = true;
	}

	void Actor::QuitGame()
	{
		// 엔진 종료 요청
		Engine::Get().Quit();
	}

	void Actor::SetPosition(const Vector2& newPosition)
	{
		// 변경하려는 위치 값이 기존 값과 동일하면 종료
		if (position == newPosition)
		{
			return;
		}

		position = newPosition;
	}
	int Actor::GetWidth() const
	{
		switch (renderType)
		{
		case Craft::ActorRenderType::Text:
			return sprite.GetWidth();

		case Craft::ActorRenderType::PixelSprite:
			return pixelSprite.GetWidth();
		}

		return 0;
	}

	int Actor::GetHeight() const
	{
		switch (renderType)
		{
		case Craft::ActorRenderType::Text:
			return sprite.GetHeight();

		case Craft::ActorRenderType::PixelSprite:
			return pixelSprite.GetHeight();
		}
		return 0;
	}

	void Actor::ChangeSprite(const Sprite& newSprite)
	{
		sprite = newSprite;

		// 랜더 타입 변경
		renderType = ActorRenderType::Text;
	}

	void Actor::ChangePixelSprite(const PixelSprite& newPixelSprite)
	{
		pixelSprite = newPixelSprite;

		// 랜더 타입 변경
		renderType = ActorRenderType::PixelSprite;
	}
}