#pragma once

#include <Actor/Actor.h>
#include <Actor/Character/CharacterPart.h>
#include <Actor/Character/CharacterStats.h>

#include <array>
#include <memory>
#include <vector>

// Character가 부위 HP를 직접 관리하기 위해 사용
struct CharacterPartHealth
{
    int maxHealth = 0;
    int currentHealth = 0;
};

class Character : public Craft::Actor
{
    TYPE_DECLARATIONS(Character, Actor)

public:
    Character(
        const Craft::Vector2F& position = Craft::Vector2F::Zero,
        const CharacterStats& stats = CharacterStats()
    );

    virtual ~Character() override = default;

public:
    // Character는 모든 CharacterPart를 각각 렌더
    virtual void Draw() override;

protected:
    // 모두 CharacterPart 포인터로 저장할 수 있게 하기 위함
    CharacterPart& AddPart(std::unique_ptr<CharacterPart> part, int maxPartHealth = 0);

    // 원하는 종류의 부위를 검색
    CharacterPart* GetPart(CharacterPartType type);

    const CharacterPart* GetPart(CharacterPartType type) const;

    // 부위 체력 관련
    // 지정 부위에 Damage 적용
    void ApplyPartDamage(CharacterPartType type, int damage);

    // 파괴된 부위 복원
    void RestorePart(CharacterPartType type);

    // 특정 부위 HP 반환
    CharacterPartHealth GetPartHealth(CharacterPartType type) const;

protected:
    // Character Bounds 설정
    void SetCharacterBounds(int width, int height);

    // 캐릭터 공통 스탯
    void SetCurrentHealth(int newHealth);

    inline const CharacterStats& GetCharacterStats() const { return stats; }

    inline int GetCurrentHealth() const { return currentHealth; }

private:
    // 실제 한 부위를 Renderer에 Submit
    void DrawPart(const CharacterPart& part);

    // enum -> array index 변환
    static size_t ToPartIndex(CharacterPartType type);

private:
    // 캐릭터 전체 스탯
    CharacterStats stats;

    // 캐릭터 전체 현재 HP
    int currentHealth = 0;

    // Character가 모든 부위의 소유권을 가짐
    // Player는 부위를 생성만 하고 AddPart로 Character에게 넘김
    std::vector<std::unique_ptr<CharacterPart>> parts;

    // 부위별 HP
    std::array<CharacterPartHealth, static_cast<size_t>(CharacterPartType::Count)> partHealth{};
};

