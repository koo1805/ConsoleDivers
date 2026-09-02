#include "CharacterPart.h"

CharacterPart::CharacterPart(
	CharacterPartType type,
	const Craft::PixelSprite& sprite,
	const Craft::Vector2& localPosition,
	int sortingOffset,
	bool canBeDestroyed,
	bool visibleWhenDestroyed)
	: type(type),
	sprite(sprite),
	localPosition(localPosition),
	sortingOffset(sortingOffset),
	canBeDestroyed(canBeDestroyed),
	visibleWhenDestroyed(visibleWhenDestroyed)
{ }

void CharacterPart::OnDestroyed()
{

}

void CharacterPart::OnRestored()
{

}

// 해당 부위를 Renderer에 제출해야 하는지 판단
bool CharacterPart::ShouldRender() const
{
	// 일반 Visible이 꺼졌거나 Sprite 자체가 비어 있다면 출력하지 않음
	if (!isVisible || sprite.IsEmpty())
	{
		return false;
	}

	// 파괴됐으며, 파괴 상태에서 표시하지 않는 부위라면 숨김
	if (isDestroyed && !visibleWhenDestroyed)
	{
		return false;
	}

	return true;
}

// Character로부터 부위 파괴 상태를 전달받음
void CharacterPart::SetDestroyed(bool destroyed)
{
	// 파괴하려고 하는데 해당 부위가 부위 파괴 기능을 사용하지 않는다면 무시
	if (destroyed && !canBeDestroyed)
	{
		return;
	}

	// 이미 동일한 상태라면 중복 처리X
	if (isDestroyed == destroyed)
	{
		return;
	}

	isDestroyed = destroyed;


	if (isDestroyed)
	{
		// 자식 클래스가 있다면 해당 클래스의 OnDestroyed가 호출
		OnDestroyed();
	}
	else
	{
		OnRestored();
	}
}
