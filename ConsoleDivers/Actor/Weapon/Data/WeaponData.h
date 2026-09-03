#pragma once

// Weapon Slot
enum class WeaponSlotType
{
    Primary,        // 주무기
    Support         // 지원무기
};

// Weapon Ammo Data
struct WeaponAmmoData
{
    // 탄창 하나의 최대 장탄수
    int magazineCapacity = 0;

    // 현재 탄창에 남아있는 탄
    int currentAmmo = 0;

    // 남은 예비 탄창 수
    int reserveMagazineCount = 0;

    // 최대 예비 탄창 수
    int maxReserveMagazineCount = 0;

    // 장전 시간
    float reloadDuration = 0.0f;
};

// Weapon Runtime Data
struct WeaponRuntimeData
{
    // 현재 장전 중인지
    bool isReloading = false;

    // 현재 장전 경과 시간
    float reloadTimer = 0.0f;
};