#include "player.h"
#include "../memory/memory.h"
#include "../memory/pattern.h"

namespace Features::Player {

static State s_State;

// AOB patterns for MGSV:TPP (64-bit)
// These target the Fox Engine's player systems.
// ?? = wildcard byte for version-independent matching.

static Memory::PatchEntry s_GodMode;
static Memory::PatchEntry s_InfAmmo;
static Memory::PatchEntry s_NoReload;
static Memory::PatchEntry s_InfSuppressor;
static Memory::PatchEntry s_NoRecoil;
static Memory::PatchEntry s_OneHitKill;
static Memory::PatchEntry s_InfReflex;

// Addresses resolved at runtime for per-frame writes
static uintptr_t s_DetectionAddr = 0;
static uintptr_t s_SpeedAddr     = 0;
static float     s_OriginalSpeed = 1.0f;

State& GetState() { return s_State; }

void Init() {
    uintptr_t base = Memory::GetBaseAddress();
    size_t    size = Memory::GetModuleSize();

    // Player health write — movss [reg+offset], xmm  (the damage application)
    uintptr_t healthWrite = Pattern::Scan(
        "F3 0F 11 89 ?? ?? ?? ?? 48 8B 41 ?? 48 85 C0", base, size
    );
    if (healthWrite) {
        s_GodMode.address = healthWrite;
        s_GodMode.patched = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
    }

    // Ammo decrement — sub [reg+offset], 1  or  dec dword ptr
    uintptr_t ammoDec = Pattern::Scan(
        "FF 88 ?? ?? ?? ?? 83 B8 ?? ?? ?? ?? 00 7F", base, size
    );
    if (ammoDec) {
        s_InfAmmo.address = ammoDec;
        s_InfAmmo.patched = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
    }

    // Reload check — cmp current_mag, 0; jle reload
    uintptr_t reloadCheck = Pattern::Scan(
        "83 B9 ?? ?? ?? ?? 00 0F 8E ?? ?? ?? ?? 48 8B", base, size
    );
    if (reloadCheck) {
        s_NoReload.address = reloadCheck;
        s_NoReload.patched = { 0x83, 0xB9, 0x00, 0x00, 0x00, 0x00, 0x01,
                               0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
    }

    // Suppressor durability decrement
    uintptr_t suppDec = Pattern::Scan(
        "F3 0F 5C ?? ?? ?? ?? ?? F3 0F 11 ?? ?? ?? ?? ?? 0F 2F", base, size
    );
    if (suppDec) {
        s_InfSuppressor.address = suppDec;
        s_InfSuppressor.patched = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
                                    0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
    }

    // Recoil vector application — addss xmm, [camera_pitch]
    uintptr_t recoilWrite = Pattern::Scan(
        "F3 0F 58 ?? ?? ?? ?? ?? F3 0F 11 ?? ?? ?? ?? ?? E8 ?? ?? ?? ?? 48 8B", base, size
    );
    if (recoilWrite) {
        s_NoRecoil.address = recoilWrite;
        s_NoRecoil.patched = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
                               0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
    }

    // Enemy health write — similar to player health but in NPC damage handler
    uintptr_t enemyHealth = Pattern::Scan(
        "F3 0F 11 ?? ?? ?? ?? ?? 80 ?? ?? ?? ?? ?? 00 74", base, size
    );
    if (enemyHealth) {
        s_OneHitKill.address = enemyHealth;
        // Overwrite movss with: xorps xmm0,xmm0; movss [reg+off],xmm0; nop padding
        s_OneHitKill.patched = { 0x0F, 0x57, 0xC0,           // xorps xmm0, xmm0
                                 0xF3, 0x0F, 0x11, 0x41, 0x00 }; // movss [rcx+0], xmm0 (placeholder)
    }

    // Reflex mode timer decrement
    uintptr_t reflexTimer = Pattern::Scan(
        "F3 0F 5C ?? ?? ?? ?? ?? F3 0F 11 ?? ?? ?? ?? ?? 0F 2F ?? ?? 76", base, size
    );
    if (reflexTimer) {
        s_InfReflex.address = reflexTimer;
        s_InfReflex.patched = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
                                0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
    }

    // Detection meter write address (for per-frame freeze)
    uintptr_t detectionScan = Pattern::Scan(
        "F3 0F 11 ?? ?? ?? ?? ?? 48 8B ?? ?? E8 ?? ?? ?? ?? 84 C0 74", base, size
    );
    if (detectionScan) {
        // Resolve the relative offset in the instruction to get the data address
        s_DetectionAddr = detectionScan;
    }

    // Player speed multiplier address
    uintptr_t speedScan = Pattern::Scan(
        "F3 0F 10 ?? ?? ?? ?? ?? F3 0F 59 ?? ?? ?? ?? ?? F3 0F 11 ?? ?? ?? ?? ?? 48 8B", base, size
    );
    if (speedScan) {
        s_SpeedAddr = speedScan;
    }
}

void Tick() {
    // Byte-patch toggles: apply or restore when state changes
    static bool prevGod = false, prevAmmo = false, prevReload = false;
    static bool prevSupp = false, prevRecoil = false, prevOHK = false, prevReflex = false;

    if (s_State.godMode != prevGod) {
        s_State.godMode ? s_GodMode.Apply() : s_GodMode.Restore();
        prevGod = s_State.godMode;
    }
    if (s_State.infiniteAmmo != prevAmmo) {
        s_State.infiniteAmmo ? s_InfAmmo.Apply() : s_InfAmmo.Restore();
        prevAmmo = s_State.infiniteAmmo;
    }
    if (s_State.noReload != prevReload) {
        s_State.noReload ? s_NoReload.Apply() : s_NoReload.Restore();
        prevReload = s_State.noReload;
    }
    if (s_State.infiniteSuppressor != prevSupp) {
        s_State.infiniteSuppressor ? s_InfSuppressor.Apply() : s_InfSuppressor.Restore();
        prevSupp = s_State.infiniteSuppressor;
    }
    if (s_State.noRecoil != prevRecoil) {
        s_State.noRecoil ? s_NoRecoil.Apply() : s_NoRecoil.Restore();
        prevRecoil = s_State.noRecoil;
    }
    if (s_State.oneHitKill != prevOHK) {
        s_State.oneHitKill ? s_OneHitKill.Apply() : s_OneHitKill.Restore();
        prevOHK = s_State.oneHitKill;
    }
    if (s_State.infiniteReflex != prevReflex) {
        s_State.infiniteReflex ? s_InfReflex.Apply() : s_InfReflex.Restore();
        prevReflex = s_State.infiniteReflex;
    }

    // Per-frame writes for continuous effects
    if (s_State.stealthMode && s_DetectionAddr) {
        float zero = 0.0f;
        Memory::Write<float>(s_DetectionAddr, zero);
    }
}

} // namespace Features::Player
