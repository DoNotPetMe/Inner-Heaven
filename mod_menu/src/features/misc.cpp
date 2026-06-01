#include "misc.h"
#include "../memory/memory.h"
#include "../memory/pattern.h"

namespace Features::Misc {

static State s_State;

static uintptr_t s_PlayerPosAddr  = 0;
static uintptr_t s_CameraPosAddr  = 0;
static uintptr_t s_CameraFlagAddr = 0;
static uintptr_t s_PlayerScaleAddr = 0;

static Memory::PatchEntry s_NoFallDamage;
static Memory::PatchEntry s_RapidFire;

State& GetState() { return s_State; }

void Init() {
    uintptr_t base = Memory::GetBaseAddress();
    size_t    size = Memory::GetModuleSize();

    // Player world position — same pattern as used in world teleport
    uintptr_t posScan = Pattern::Scan(
        "F3 0F 11 ?? ?? ?? ?? ?? F3 0F 11 ?? ?? ?? ?? ?? F3 0F 11 ?? ?? ?? ?? ?? 48 8B ?? ?? E8", base, size
    );
    if (posScan) {
        s_PlayerPosAddr = posScan;
    }

    // Camera position vector (for free cam)
    uintptr_t camScan = Pattern::Scan(
        "F3 44 0F 11 ?? ?? ?? ?? ?? F3 44 0F 11 ?? ?? ?? ?? ?? F3 44 0F 11 ?? ?? ?? ?? ?? 48 8B", base, size
    );
    if (camScan) {
        s_CameraPosAddr = camScan;
    }

    // Camera follow mode flag
    uintptr_t camFlagScan = Pattern::Scan(
        "C6 81 ?? ?? ?? ?? 01 48 8B ?? ?? ?? ?? ?? 48 85 C9 74 ?? E8", base, size
    );
    if (camFlagScan) {
        s_CameraFlagAddr = camFlagScan;
    }

    // Fall damage application (damage from falling height)
    uintptr_t fallScan = Pattern::Scan(
        "F3 0F 11 ?? ?? ?? ?? ?? EB ?? F3 0F 10 ?? ?? ?? ?? ?? 0F 2F C1 76 ?? F3 0F 5C", base, size
    );
    if (fallScan) {
        s_NoFallDamage.address = fallScan;
        s_NoFallDamage.patched = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
    }

    // Fire rate limiter — timer comparison before allowing next shot
    uintptr_t fireScan = Pattern::Scan(
        "0F 2F ?? ?? ?? ?? ?? 73 ?? F3 0F 58", base, size
    );
    if (fireScan) {
        s_RapidFire.address = fireScan;
        // Patch the comiss comparison to always fall through (jmp over the wait)
        s_RapidFire.patched = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0xEB };
    }

    // Player scale
    uintptr_t scaleScan = Pattern::Scan(
        "F3 0F 10 ?? ?? ?? ?? ?? F3 0F 59 ?? ?? F3 0F 11 ?? ?? ?? ?? ?? 48 8B ?? E8", base, size
    );
    if (scaleScan) {
        s_PlayerScaleAddr = scaleScan;
    }
}

void SavePosition() {
    if (!s_PlayerPosAddr) return;
    s_State.savedX = Memory::Read<float>(s_PlayerPosAddr);
    s_State.savedY = Memory::Read<float>(s_PlayerPosAddr + 4);
    s_State.savedZ = Memory::Read<float>(s_PlayerPosAddr + 8);
    s_State.hasSavedPos = true;
}

void LoadPosition() {
    if (!s_PlayerPosAddr || !s_State.hasSavedPos) return;
    Memory::Write<float>(s_PlayerPosAddr, s_State.savedX);
    Memory::Write<float>(s_PlayerPosAddr + 4, s_State.savedY);
    Memory::Write<float>(s_PlayerPosAddr + 8, s_State.savedZ);
}

void Tick() {
    // No fall damage toggle
    static bool prevFall = false;
    if (s_State.noFallDamage != prevFall) {
        s_State.noFallDamage ? s_NoFallDamage.Apply() : s_NoFallDamage.Restore();
        prevFall = s_State.noFallDamage;
    }

    // Rapid fire toggle
    static bool prevRapid = false;
    if (s_State.rapidFire != prevRapid) {
        s_State.rapidFire ? s_RapidFire.Apply() : s_RapidFire.Restore();
        prevRapid = s_State.rapidFire;
    }

    // Custom player scale
    if (s_State.customScale && s_PlayerScaleAddr) {
        Memory::Write<float>(s_PlayerScaleAddr, s_State.scaleValue);
    }

    // Free camera movement handled per-frame via input
    if (s_State.freeCamera && s_CameraPosAddr) {
        float speed = s_State.camSpeed * 0.016f; // ~60fps timestep

        if (GetAsyncKeyState('W') & 0x8000)
            Memory::Write<float>(s_CameraPosAddr + 8, Memory::Read<float>(s_CameraPosAddr + 8) + speed);
        if (GetAsyncKeyState('S') & 0x8000)
            Memory::Write<float>(s_CameraPosAddr + 8, Memory::Read<float>(s_CameraPosAddr + 8) - speed);
        if (GetAsyncKeyState('A') & 0x8000)
            Memory::Write<float>(s_CameraPosAddr, Memory::Read<float>(s_CameraPosAddr) - speed);
        if (GetAsyncKeyState('D') & 0x8000)
            Memory::Write<float>(s_CameraPosAddr, Memory::Read<float>(s_CameraPosAddr) + speed);
        if (GetAsyncKeyState(VK_SPACE) & 0x8000)
            Memory::Write<float>(s_CameraPosAddr + 4, Memory::Read<float>(s_CameraPosAddr + 4) + speed);
        if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
            Memory::Write<float>(s_CameraPosAddr + 4, Memory::Read<float>(s_CameraPosAddr + 4) - speed);
    }
}

} // namespace Features::Misc
