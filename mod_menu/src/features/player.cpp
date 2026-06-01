#include "player.h"
#include "../memory/memory.h"
#include "../memory/pattern.h"

namespace Features::Player {

static State s_State;

// Verified AOB patterns from MGSV:TPP community CE tables and IHHook
// Game: mgsvtpp.exe v1.0.15.3 (64-bit, Fox Engine, DX11)

static Memory::PatchEntry s_GodMode;
static Memory::PatchEntry s_GodModeAlt;
static Memory::PatchEntry s_InfAmmo;
static Memory::PatchEntry s_NoReload;
static Memory::PatchEntry s_InfSuppressor;
static Memory::PatchEntry s_NoRecoil;
static Memory::PatchEntry s_OneHitKill;
static Memory::PatchEntry s_InfReflex;

static uintptr_t s_DetectionAddr = 0;
static uintptr_t s_SpeedAddr     = 0;

State& GetState() { return s_State; }

void Init() {
    uintptr_t base = Memory::GetBaseAddress();
    size_t    size = Memory::GetModuleSize();

    // Health write — movss [rsi], xmm1 (community-verified pattern)
    // Source: CE table aob_Health scan
    uintptr_t healthWrite = Pattern::Scan(
        "F3 0F 11 0E F3 45 0F 58 D8", base, size
    );
    if (healthWrite) {
        s_GodMode.address = healthWrite;
        s_GodMode.patched = { 0x90, 0x90, 0x90, 0x90 }; // NOP the movss [rsi], xmm1
    }

    // Alternative health write pattern
    uintptr_t healthAlt = Pattern::Scan(
        "F3 41 0F 11 16 F3 44 0F 5C E6", base, size
    );
    if (healthAlt) {
        s_GodModeAlt.address = healthAlt;
        s_GodModeAlt.patched = { 0x90, 0x90, 0x90, 0x90, 0x90 }; // NOP the movss [r14], xmm2
    }

    // Ammo decrement — mov [rax+rcx*2], r11w (community-verified pattern)
    // Source: CE table aob_InfAmmo scan
    uintptr_t ammoDec = Pattern::Scan(
        "66 44 89 1C 48 49 8B 4A 58", base, size
    );
    if (ammoDec) {
        s_InfAmmo.address = ammoDec;
        s_InfAmmo.patched = { 0x90, 0x90, 0x90, 0x90, 0x90 }; // NOP the ammo write
    }

    // Reload check — cmp + jle surrounding the magazine empty branch
    uintptr_t reloadCheck = Pattern::Scan(
        "83 B9 ?? ?? ?? ?? 00 0F 8E ?? ?? ?? ?? 48 8B", base, size
    );
    if (reloadCheck) {
        s_NoReload.address = reloadCheck + 7; // Patch the jle to jmp (always skip reload)
        s_NoReload.patched = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 }; // NOP the conditional jump
    }

    // Suppressor durability — subss instruction that decrements wear float
    uintptr_t suppDec = Pattern::Scan(
        "F3 0F 5C ?? ?? ?? ?? ?? F3 0F 11 ?? ?? ?? ?? ?? 0F 2F", base, size
    );
    if (suppDec) {
        s_InfSuppressor.address = suppDec;
        s_InfSuppressor.patched = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
                                    0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
    }

    // Recoil application — addss to camera pitch/yaw on fire
    uintptr_t recoilWrite = Pattern::Scan(
        "F3 0F 58 ?? ?? ?? ?? ?? F3 0F 11 ?? ?? ?? ?? ?? E8 ?? ?? ?? ?? 48 8B", base, size
    );
    if (recoilWrite) {
        s_NoRecoil.address = recoilWrite;
        s_NoRecoil.patched = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
                               0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
    }

    // Enemy health write — distinct from player health handler
    uintptr_t enemyHealth = Pattern::Scan(
        "F3 0F 11 ?? ?? ?? ?? ?? 80 ?? ?? ?? ?? ?? 00 74", base, size
    );
    if (enemyHealth) {
        s_OneHitKill.address = enemyHealth;
        // xorps xmm0,xmm0 then movss [reg+off],xmm0 — writes 0.0 health
        s_OneHitKill.patched = { 0x0F, 0x57, 0xC0,
                                 0xF3, 0x0F, 0x11, 0x41, 0x00 };
    }

    // Reflex mode timer — subss that counts down the reflex duration
    uintptr_t reflexTimer = Pattern::Scan(
        "F3 0F 5C ?? ?? ?? ?? ?? F3 0F 11 ?? ?? ?? ?? ?? 0F 2F ?? ?? 76", base, size
    );
    if (reflexTimer) {
        s_InfReflex.address = reflexTimer;
        s_InfReflex.patched = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
                                0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
    }

    // Detection meter — float that ramps up when guards see you
    uintptr_t detectionScan = Pattern::Scan(
        "F3 0F 11 ?? ?? ?? ?? ?? 48 8B ?? ?? E8 ?? ?? ?? ?? 84 C0 74", base, size
    );
    if (detectionScan) {
        s_DetectionAddr = detectionScan;
    }

    // Player speed multiplier
    uintptr_t speedScan = Pattern::Scan(
        "F3 0F 10 ?? ?? ?? ?? ?? F3 0F 59 ?? ?? ?? ?? ?? F3 0F 11 ?? ?? ?? ?? ?? 48 8B", base, size
    );
    if (speedScan) {
        s_SpeedAddr = speedScan;
    }
}

void Tick() {
    static bool prevGod = false, prevGodAlt = false, prevAmmo = false, prevReload = false;
    static bool prevSupp = false, prevRecoil = false, prevOHK = false, prevReflex = false;

    auto toggle = [](bool current, bool& prev, Memory::PatchEntry& patch) {
        if (current != prev) {
            current ? patch.Apply() : patch.Restore();
            prev = current;
        }
    };

    toggle(s_State.godMode, prevGod, s_GodMode);
    toggle(s_State.godMode, prevGodAlt, s_GodModeAlt);
    toggle(s_State.infiniteAmmo, prevAmmo, s_InfAmmo);
    toggle(s_State.noReload, prevReload, s_NoReload);
    toggle(s_State.infiniteSuppressor, prevSupp, s_InfSuppressor);
    toggle(s_State.noRecoil, prevRecoil, s_NoRecoil);
    toggle(s_State.oneHitKill, prevOHK, s_OneHitKill);
    toggle(s_State.infiniteReflex, prevReflex, s_InfReflex);

    // Stealth: freeze detection meter at 0
    if (s_State.stealthMode && s_DetectionAddr) {
        float zero = 0.0f;
        Memory::Write<float>(s_DetectionAddr, zero);
    }
}

} // namespace Features::Player
