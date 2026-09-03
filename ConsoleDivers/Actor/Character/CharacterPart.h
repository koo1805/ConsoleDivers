#pragma once

#include <Math/Vector2.h>
#include <Render/Sprite/PixelSprite.h>

// 캐릭터를 구성하는 부위 종류
enum class CharacterPartType
{
    Head,
    Body,
    Legs,
    LeftHand,
    RightHand,
    Count           // Count는 Character 내부의 부위 상태 배열 크기 계산에 사용
};

class CharacterPart
{
public:
    CharacterPart() = default;

    CharacterPart(
        CharacterPartType type,
        const Craft::PixelSprite& sprite,
        const Craft::Vector2& localPosition,
        int sortingOffset = 0,
        bool canBeDestroyed = false,
        bool visibleWhenDestroyed = false);

    virtual ~CharacterPart() = default;

public:
    virtual void OnDestroyed();
    virtual void OnRestored();

    // 최종적으로 이 부위를 렌더링해야 하는지 판단
    bool ShouldRender() const;

    // Getter
    // 캐릭터 부위 반환
    inline CharacterPartType GetType() const { return type; }

    // 해당 부위 Sprite 반환
    inline const Craft::PixelSprite& GetSprite() const { return sprite; }

    // Character 중심 기준 상대 위치 반환
    inline Craft::Vector2 GetLocalPosition() const { return localPosition; }

    // 해당 부위의 sortingOrder값
    inline int GetSortingOffset() const { return sortingOffset; }

    // 부위 파괴 사용 여부
    inline bool CanBeDestroyed() const { return canBeDestroyed; }
    
    // 파괴됨
    inline bool IsDestroyed() const { return isDestroyed; }

    // Sprite표시 상태 여부
    inline bool IsVisible() const { return isVisible; }
    
    // 파괴 상태에서도 Sprite 표시 여부
    inline bool IsVisibleWhenDestroyed() const { return visibleWhenDestroyed; }

    // Setter
    inline void SetSprite(const Craft::PixelSprite& newSprite) { sprite = newSprite; }

    inline void SetLocalPosition(const Craft::Vector2& newPosition) { localPosition = newPosition; }

    inline void SetSortingOffset(int newSortingOffset) { sortingOffset = newSortingOffset; }

    inline void SetVisible(bool visible) { isVisible = visible; }

    inline void SetCanBeDestroyed(bool canDestroy) { canBeDestroyed = canDestroy; }

    inline void SetVisibleWhenDestroyed(bool visible) { visibleWhenDestroyed = visible; }

    void SetDestroyed(bool destroyed);

private:
    CharacterPartType type = CharacterPartType::Body;

    Craft::PixelSprite sprite;

    // Character 위치 기준 상대 위치
    Craft::Vector2 localPosition = Craft::Vector2::Zero;

    int sortingOffset = 0;

    bool isVisible = true;

    bool canBeDestroyed = false;

    bool isDestroyed = false;

    bool visibleWhenDestroyed = false;
};

