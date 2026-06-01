#include "world.h"
#include "../memory/memory.h"
#include "../memory/pattern.h"

namespace Features::World {

static State s_State;

static uintptr_t s_TimeScaleAddr   = 0;
static uintptr_t s_TimeOfDayAddr   = 0;
static uintptr_t s_WeatherAddr     = 0;
static uintptr_t s_PlayerPosAddr   = 0;
static uintptr_t s_WaypointAddr    = 0;
static uintptr_t s_AIUpdateAddr    = 0;
static uintptr_t s_FultonCountAddr = 0;

static float s_OrigTimeScale = 1.0f;
static Memory::PatchEntry s_AIDisable;
static Memory::PatchEntry s_FultonPatch;

State& GetState() { return s_State; }

void Init() {
    uintptr_t base = Memory::GetBaseAddress();
    size_t    size = Memory::GetModuleSize();

    // Global time scale (Fox Engine simulation rate)
    uintptr_t tsScan = Pattern::Scan(
        "F3 0F 10 ?? ?? ?? ?? ?? F3 0F 59 ?? ?? F3 0F 11 ?? ?? ?? ?? ?? C3", base, size
    );
    if (tsScan) {
        s_TimeScaleAddr = tsScan;
    }

    // In-game clock / time-of-day float (0.0–24.0)
    uintptr_t todScan = Pattern::Scan(
        "F3 0F 11 ?? ?? ?? ?? ?? 0F 2F ?? ?? ?? ?? ?? 72 ?? F3 0F 10", base, size
    );
    if (todScan) {
        s_TimeOfDayAddr = todScan;
    }

    // Weather state integer
    uintptr_t weatherScan = Pattern::Scan(
        "89 ?? ?? ?? ?? ?? 83 ?? ?? ?? ?? ?? 04 72", base, size
    );
    if (weatherScan) {
        s_WeatherAddr = weatherScan;
    }

    // Player position vector (3 floats: x, y, z)
    uintptr_t posScan = Pattern::Scan(
        "F3 0F 11 ?? ?? ?? ?? ?? F3 0F 11 ?? ?? ?? ?? ?? F3 0F 11 ?? ?? ?? ?? ?? 48 8B ?? ?? E8", base, size
    );
    if (posScan) {
        s_PlayerPosAddr = posScan;
    }

    // Waypoint/marker world coordinates
    uintptr_t wpScan = Pattern::Scan(
        "F3 0F 10 ?? ?? ?? ?? ?? F3 0F 10 ?? ?? ?? ?? ?? F3 0F 10 ?? ?? ?? ?? ?? E8 ?? ?? ?? ?? 84 C0", base, size
    );
    if (wpScan) {
        s_WaypointAddr = wpScan;
    }

    // Enemy AI behavior tree tick — call instruction that drives all NPC logic
    uintptr_t aiScan = Pattern::Scan(
        "E8 ?? ?? ?? ?? 48 8B ?? ?? 48 85 C9 74 ?? E8 ?? ?? ?? ?? 84 C0 74", base, size
    );
    if (aiScan) {
        s_AIDisable.address = aiScan;
        s_AIDisable.patched = { 0x90, 0x90, 0x90, 0x90, 0x90 }; // NOP the call
    }

    // Fulton count/limit check
    uintptr_t fultonScan = Pattern::Scan(
        "FF 88 ?? ?? ?? ?? 83 B8 ?? ?? ?? ?? 00 75", base, size
    );
    if (fultonScan) {
        s_FultonPatch.address = fultonScan;
        s_FultonPatch.patched = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 }; // NOP the dec
    }
}

void TeleportToWaypoint() {
    if (!s_PlayerPosAddr || !s_WaypointAddr) return;

    float wx = Memory::Read<float>(s_WaypointAddr);
    float wy = Memory::Read<float>(s_WaypointAddr + 4);
    float wz = Memory::Read<float>(s_WaypointAddr + 8);

    Memory::Write<float>(s_PlayerPosAddr, wx);
    Memory::Write<float>(s_PlayerPosAddr + 4, wy);
    Memory::Write<float>(s_PlayerPosAddr + 8, wz);
}

void Tick() {
    // Slow motion toggle
    static bool prevSlowMo = false;
    if (s_State.slowMotion != prevSlowMo) {
        if (s_TimeScaleAddr) {
            if (s_State.slowMotion)
                Memory::Write<float>(s_TimeScaleAddr, s_State.timeScale);
            else
                Memory::Write<float>(s_TimeScaleAddr, s_OrigTimeScale);
        }
        prevSlowMo = s_State.slowMotion;
    }
    if (s_State.slowMotion && s_TimeScaleAddr) {
        Memory::Write<float>(s_TimeScaleAddr, s_State.timeScale);
    }

    // Time of day override
    if (s_State.overrideTime && s_TimeOfDayAddr) {
        Memory::Write<float>(s_TimeOfDayAddr, s_State.timeOfDay);
    }

    // Weather override
    if (s_State.overrideWeather && s_WeatherAddr) {
        Memory::Write<int>(s_WeatherAddr, static_cast<int>(s_State.weather));
    }

    // No enemy AI toggle
    static bool prevAI = false;
    if (s_State.noEnemyAI != prevAI) {
        s_State.noEnemyAI ? s_AIDisable.Apply() : s_AIDisable.Restore();
        prevAI = s_State.noEnemyAI;
    }

    // Infinite fulton toggle
    static bool prevFulton = false;
    if (s_State.infiniteFulton != prevFulton) {
        s_State.infiniteFulton ? s_FultonPatch.Apply() : s_FultonPatch.Restore();
        prevFulton = s_State.infiniteFulton;
    }

    // Teleport trigger (one-shot)
    if (s_State.teleportToWP) {
        TeleportToWaypoint();
        s_State.teleportToWP = false;
    }
}

} // namespace Features::World
