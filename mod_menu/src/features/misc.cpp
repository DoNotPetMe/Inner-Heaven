#include "misc.h"
#include "../config.h"
#include "../memory/memory.h"
#include "../memory/pattern.h"

namespace Features::Misc {

static uintptr_t s_PlayerPosAddr = 0;
static uintptr_t s_CameraPosAddr = 0;

void Init() {
    uintptr_t base = Memory::GetBaseAddress();
    size_t    size = Memory::GetModuleSize();

    uintptr_t pos = Pattern::Scan("F3 0F 11 ?? ?? ?? ?? ?? F3 0F 11 ?? ?? ?? ?? ?? F3 0F 11 ?? ?? ?? ?? ?? 48 8B ?? ?? E8", base, size);
    if (pos) s_PlayerPosAddr = pos;

    uintptr_t cam = Pattern::Scan("F3 44 0F 11 ?? ?? ?? ?? ?? F3 44 0F 11 ?? ?? ?? ?? ?? F3 44 0F 11 ?? ?? ?? ?? ?? 48 8B", base, size);
    if (cam) s_CameraPosAddr = cam;
}

void SavePosition() {
    if (!s_PlayerPosAddr) return;
    auto& c = Config::Get();
    c.savedX = Memory::Read<float>(s_PlayerPosAddr);
    c.savedY = Memory::Read<float>(s_PlayerPosAddr + 4);
    c.savedZ = Memory::Read<float>(s_PlayerPosAddr + 8);
    c.hasSavedPos = true;
}

void LoadPosition() {
    auto& c = Config::Get();
    if (!s_PlayerPosAddr || !c.hasSavedPos) return;
    Memory::Write<float>(s_PlayerPosAddr, c.savedX);
    Memory::Write<float>(s_PlayerPosAddr + 4, c.savedY);
    Memory::Write<float>(s_PlayerPosAddr + 8, c.savedZ);
}

void Tick() {
    auto& c = Config::Get();

    if (c.customPlayerScale && s_PlayerPosAddr) {
        // Player scale would be written to the scale field if found
    }

    if (c.freeCamera && s_CameraPosAddr) {
        float speed = c.camSpeed * 0.016f;
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

float* GetPlayerPosPtr() {
    return s_PlayerPosAddr ? reinterpret_cast<float*>(s_PlayerPosAddr) : nullptr;
}

PosRef GetPlayerPos() {
    return { GetPlayerPosPtr() };
}

} // namespace Features::Misc
