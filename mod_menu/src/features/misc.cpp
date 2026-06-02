#include "misc.h"
#include "../config.h"
#include "../memory/memory.h"
#include "../memory/pattern.h"
#include "game_lua.h"
#include <cstdio>
#include <windows.h>

namespace Features::Misc {

// ── Teleport / position, the VERIFIED way ────────────────────────────────────
// The old implementation poked an AOB-scanned "player position" address. That
// scan is a guess and on current builds resolves to the wrong floats (or none),
// so save/load did nothing. This version goes through the connected Lua bridge
// using functions verified against the deminified game scripts + IHHook:
//
//   read pos : vars.playerPosX / playerPosY / playerPosZ, vars.playerRotY
//   warp     : TppPlayer.Warp{pos={x,y,z}, rotY=degrees}
//              (internally GameObject.SendCommand({type="TppPlayer2",index=..},
//               {id="WarpAndWaitBlock", pos=.., rotY=..}))
//
// Saved coordinates are kept BOTH in C++ Config (read back at full float
// precision via RunCodeFloat) and re-applied via Warp, so a slot survives even
// if the Lua state is reset between loads.

static uintptr_t s_CameraPosAddr = 0;   // still memory-based (free-cam nudging)

void Init() {
    uintptr_t base = Memory::GetBaseAddress();
    size_t    size = Memory::GetModuleSize();

    uintptr_t cam = Pattern::Scan("F3 44 0F 11 ?? ?? ?? ?? ?? F3 44 0F 11 ?? ?? ?? ?? ?? F3 44 0F 11 ?? ?? ?? ?? ?? 48 8B", base, size);
    if (cam) s_CameraPosAddr = cam;
}

// Read the live player position from the game (Lua). Returns false if the
// bridge isn't ready or the read failed.
bool ReadPlayerPos(float out[4]) {
    if (!GameLua::IsReady()) return false;
    // Gate on availability first: outside a mission `vars` is nil and a naive
    // read would yield (0,0,0) and warp us to the world origin.
    if (GameLua::RunCodeInt("return (vars and vars.playerPosX ~= nil) and 1 or 0", 0) != 1)
        return false;
    out[0] = GameLua::RunCodeFloat("return vars.playerPosX", 0.0f);
    out[1] = GameLua::RunCodeFloat("return vars.playerPosY", 0.0f);
    out[2] = GameLua::RunCodeFloat("return vars.playerPosZ", 0.0f);
    out[3] = GameLua::RunCodeFloat("return vars.playerRotY or 0", 0.0f);
    return true;
}

// Warp the player to a world position via the verified Lua Warp command.
void WarpTo(float x, float y, float z, float rotYDeg) {
    if (!GameLua::IsReady()) return;
    char buf[256];
    snprintf(buf, sizeof(buf),
        "pcall(function() "
        "if TppPlayer and TppPlayer.Warp then "
        "TppPlayer.Warp{pos={%.3f,%.3f,%.3f},rotY=%.3f} "
        "else GameObject.SendCommand("
        "{type=\"TppPlayer2\",index=PlayerInfo.GetLocalPlayerIndex()},"
        "{id=\"WarpAndWaitBlock\",pos={%.3f,%.3f,%.3f},rotY=math.rad(%.3f)}) end end)",
        x, y, z, rotYDeg, x, y, z, rotYDeg);
    GameLua::RunCode(buf);
}

// ── Position slots (1-3) ─────────────────────────────────────────────────────
void SaveSlot(int slot) {
    auto& c = Config::Get();
    float p[4];
    if (!ReadPlayerPos(p)) return;
    switch (slot) {
        case 1: c.savedX = p[0]; c.savedY = p[1]; c.savedZ = p[2]; c.hasSavedPos  = true; break;
        case 2: c.savedPos2X = p[0]; c.savedPos2Y = p[1]; c.savedPos2Z = p[2]; c.hasSavedPos2 = true; break;
        case 3: c.savedPos3X = p[0]; c.savedPos3Y = p[1]; c.savedPos3Z = p[2]; c.hasSavedPos3 = true; break;
        default: break;
    }
}

void TeleportSlot(int slot) {
    auto& c = Config::Get();
    switch (slot) {
        case 1: if (c.hasSavedPos)  WarpTo(c.savedX, c.savedY, c.savedZ, 0.0f); break;
        case 2: if (c.hasSavedPos2) WarpTo(c.savedPos2X, c.savedPos2Y, c.savedPos2Z, 0.0f); break;
        case 3: if (c.hasSavedPos3) WarpTo(c.savedPos3X, c.savedPos3Y, c.savedPos3Z, 0.0f); break;
        default: break;
    }
}

void SavePosition() { SaveSlot(1); }
void LoadPosition() { TeleportSlot(1); }

void Tick() {
    auto& c = Config::Get();

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

// Back-compat: a few callers want the player position as a value. Prefer
// ReadPlayerPos (Lua). The pointer-based accessor is retained but now returns
// a pointer into a static buffer refreshed from the Lua read, so it no longer
// depends on a guessed AOB address.
static float s_PosCache[3] = { 0, 0, 0 };
float* GetPlayerPosPtr() {
    float p[4];
    if (ReadPlayerPos(p)) { s_PosCache[0] = p[0]; s_PosCache[1] = p[1]; s_PosCache[2] = p[2]; return s_PosCache; }
    return nullptr;
}

PosRef GetPlayerPos() {
    return { GetPlayerPosPtr() };
}

} // namespace Features::Misc
