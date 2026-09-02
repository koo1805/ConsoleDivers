#include "Character.h"
#include <Render/Renderer.h>

#include <algorithm>
#include <cassert>

using namespace Craft;
Character::Character(const Craft::Vector2F& position, const CharacterStats& stats)
	: Actor(position), stats(stats), currentHealth(stats.maxHealth)
{ }

// 모든 CharacterPart 렌더링
void Character::Draw()
{
    if (!IsActive())
    {
        return;
    }

    // 가지고 있는 모든 부위를 순회한다.
    for (const std::unique_ptr<CharacterPart>& part : parts)
    {
        if (part)
        {
            DrawPart(*part);
        }
    }
}

// 새로운 부위 추가
CharacterPart& Character::AddPart(std::unique_ptr<CharacterPart> part, int maxPartHealth)
{
    assert(part && "Character::AddPart requires a valid part.");
	
    // unique_ptr를 이동하기 전에 원본 객체 주소를 보관
    CharacterPart* rawPart = part.get();

    // 부위 타입을 배열 인덱스로 변환
    const size_t index = ToPartIndex(rawPart->GetType());

    // 부위 최대 체력 설정
    partHealth[index].maxHealth = (std::max)(0, maxPartHealth);

    // 생성 시 현재 체력 = 최대 체력
    partHealth[index].currentHealth = partHealth[index].maxHealth;

    // Character가 부위 소유권을 가져옴
    parts.push_back(std::move(part));

    // 필요하면 Player에서 생성 직후 해당 CharacterPart를 조작할 수 있도록 참조 반환
    return *rawPart;
}

// 지정한 타입의 CharacterPart 검색
CharacterPart* Character::GetPart(CharacterPartType type)
{
    for (const std::unique_ptr<CharacterPart>& part : parts)
    {
        if (part && part->GetType() == type)
        {
            return part.get();
        }
    }

    return nullptr;
}

const CharacterPart* Character::GetPart(CharacterPartType type) const
{
    for (const std::unique_ptr<CharacterPart>& part : parts)
    {
        if (part && part->GetType() == type)
        {
            return part.get();
        }
    }

    return nullptr;
}

// 부위 Damage 처리
void Character::ApplyPartDamage(CharacterPartType type, int damage)
{
    if (damage <= 0)
    {
        return;
    }

    CharacterPart* part = GetPart(type);

    if (!part)
    {
        return;
    }

    const size_t index = ToPartIndex(type);

    CharacterPartHealth& health = partHealth[index];

    // maxHealth == 0이라면 해당 부위는 현재 HP 시스템을 사용하지 않는 것으로 처리
    if (health.maxHealth <= 0)
    {
        return;
    }

    // HP 감소
    health.currentHealth = (std::max)(0, health.currentHealth - damage);

    // HP가 0이 됐을시 CharacterPart에 파괴 상태 전달
    if (health.currentHealth == 0 && part->CanBeDestroyed())
    {
        part->SetDestroyed(true);
    }
}

// 부위 복구
void Character::RestorePart(CharacterPartType type)
{
    CharacterPart* part = GetPart(type);

    if (!part)
    {
        return;
    }

    const size_t index = ToPartIndex(type);

    CharacterPartHealth& health = partHealth[index];

    // 체력 최대치 복구
    health.currentHealth = health.maxHealth;

    // CharacterPart 파괴 상태 해제
    part->SetDestroyed(false);
}

// 부위 HP 정보 반환
CharacterPartHealth Character::GetPartHealth(CharacterPartType type) const
{
	return partHealth[ToPartIndex(type)];
}

// Actor와 기존 카메라/월드 Bounds 시스템 호환용
void Character::SetCharacterBounds(int width, int height)
{
    // 실제 렌더링하지 않을 투명 Sprite를 Actor 기본 PixelSprite로 설정
    //Actor::GetWidth() | Actor::GetHeight() 기능을 그대로 사용할 수 있음
    ChangePixelSprite(Craft::PixelSprite(width, height));
}

// 캐릭터 현재 HP 변경
void Character::SetCurrentHealth(int newHealth)
{
    currentHealth = std::clamp(newHealth, 0, stats.maxHealth);
}

void Character::DrawPart(const CharacterPart& part)
{
    // CharacterPart 스스로 현재 출력 대상인지 판단
    if (!part.ShouldRender())
    {
        return;
    }

    // Character의 실제 월드 위치
    const Craft::Vector2 actorPosition(static_cast<int>(position.x), static_cast<int>(position.y));

    // 해당 부위의 상대 위치
    const Craft::Vector2 localPosition = part.GetLocalPosition();

    // 최종 부위 위치 = 캐릭터 월드 위치 + 캐릭터 로컬 위치
    const Craft::Vector2 renderPosition(actorPosition.x + localPosition.x, actorPosition.y + localPosition.y);

    // 최종 SortingOrder = 캐릭터 sortingOrder + 해당 부위 sortingOffset
    Craft::Renderer::Get().Submit(
        part.GetSprite(),
        renderPosition,
        sortingOrder +
        part.GetSortingOffset());
}

// 캐릭터 부위 타입 -> array index
size_t Character::ToPartIndex(CharacterPartType type)
{
	return size_t();
}
